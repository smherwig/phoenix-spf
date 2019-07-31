#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <rho/rho.h>

#define SPF_TRACE_ROOTDIR "/sys/kernel/debug/tracing/"

struct spf_tracer {
    int fd;
};

static bool spf_keep_running = true;

static void
spf_tracefile_do_write(const char *path, const char *val, bool append)
{
    int fd = 0;
    int flags = O_WRONLY;
    size_t val_size = 0;
    ssize_t n = 0;

    RHO_TRACE_ENTER("path=\"%s\", val=\"%s\", append=%d", path, val, append);

    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;

    fd = open(path, flags);
    if (fd == -1)
        rho_errno_die(errno, "open(\"%s\") failed", path);

    val_size = strlen(val);
    n = write(fd, val, val_size);
    if (n == -1)
        rho_errno_die(errno, "write(\"%s\", \"%s\") failed", path, val);
    else if (n != (ssize_t)val_size)
        rho_die("write(\"%s\", \"%s\") wrote %zd bytes; expected %zu",
                path, val, n, val_size);

    (void)close(fd);

    RHO_TRACE_EXIT();
    return;
}

static void
spf_tracer_write_file(struct spf_tracer *tracer,
        const char *relpath, const char *val)
{
    char fullpath[512] = { 0 };

    RHO_TRACE_ENTER();

    rho_path_join(SPF_TRACE_ROOTDIR, relpath, fullpath, sizeof(fullpath));
    spf_tracefile_do_write(fullpath, val, false);

    RHO_TRACE_EXIT();
    return;
}

static void
spf_tracer_append_file(struct spf_tracer *tracer,
        const char *relpath, const char *val)
{
    char fullpath[512] = { 0 };

    RHO_TRACE_ENTER();

    rho_path_join(SPF_TRACE_ROOTDIR, relpath, fullpath, sizeof(fullpath));
    spf_tracefile_do_write(fullpath, val, true);

    RHO_TRACE_EXIT();
    return;
}

static void
spf_tracer_reset(struct spf_tracer *tracer)
{
    RHO_TRACE_ENTER();

    spf_tracer_write_file(tracer, "current_tracer", "nop");
    spf_tracer_write_file(tracer, "tracing_on", "0");
    spf_tracer_write_file(tracer, "kprobe_events", " ");
    spf_tracer_write_file(tracer, "set_ftrace_filter", " ");
    spf_tracer_write_file(tracer, "set_ftrace_notrace", " ");
    spf_tracer_write_file(tracer, "set_graph_function", " ");
    spf_tracer_write_file(tracer, "set_graph_notrace", " ");

    RHO_TRACE_EXIT();
    return;
}

static void
spf_tracer_init(struct spf_tracer *tracer)
{
    RHO_TRACE_ENTER();

    spf_tracer_write_file(tracer, "trace", "0");
    spf_tracer_reset(tracer);

    RHO_TRACE_EXIT();
}

static void
spf_tracer_readfd(struct spf_tracer *tracer)
{
    ssize_t n = 0;
    char buf[4097] = { 0 };

    RHO_TRACE_ENTER();

    while (spf_keep_running) {
        n = read(tracer->fd, buf, sizeof(buf) - 1);
        if (n <= 0)
            break;
        buf[n] = '\0';
        printf("%s\n", buf);
    }

    RHO_TRACE_EXIT();
    return;
}

static void
spf_tracer_runloop(struct spf_tracer *tracer)
{
    int error = 0;
    fd_set fdset = {};

    RHO_ASSERT(tracer->fd >= 0);
    RHO_TRACE_ENTER();

    while (spf_keep_running) {
        FD_ZERO(&fdset);
        FD_SET(tracer->fd, &fdset);
        error = select(tracer->fd + 1, &fdset, NULL, NULL, NULL);
        if (error == -1) {
            if (errno == EINTR)
                break;  /* CTRL-C */
            else
                rho_errno_die(errno, "select");
        }

        if (FD_ISSET(tracer->fd, &fdset))
            spf_tracer_readfd(tracer);
    }

    RHO_TRACE_EXIT();
    return;
}

/********************************/

struct spf_tracer *
spf_tracer_create(void)
{
    struct spf_tracer *tracer = NULL;
    tracer = rhoL_zalloc(sizeof(*tracer));
    spf_tracer_init(tracer);
    return (tracer);
}

void
spf_tracer_destroy(struct spf_tracer *tracer)
{
    RHO_TRACE_ENTER();

    rhoL_free(tracer);

    RHO_TRACE_EXIT();
    return;
}

static void
spf_tracer_make_instance_dir(struct spf_tracer *tracer)
{
    int error = 0;
    char instance_path[512] = { 0 };
    struct stat sb = {};

    RHO_TRACE_ENTER();

    rho_path_join(SPF_TRACE_ROOTDIR, "instances/spf", instance_path,
            sizeof(instance_path));

    if ((stat(instance_path, &sb) == 0) && S_ISDIR(sb.st_mode)) {
        error = rmdir(instance_path);
        if (error == -1)
            rho_errno_die(errno, "rmdir(\"%s\")", instance_path);
    }

    error = mkdir(instance_path, 0766);
    if (error == -1)
        rho_errno_die(errno, "mkdir(\"%s\")", instance_path);

    RHO_TRACE_EXIT();
    return;
}

void
spf_tracer_start(struct spf_tracer *tracer)
{
    char pipe_path[512] = { 0 };

    RHO_TRACE_ENTER();

    spf_tracer_make_instance_dir(tracer);

    spf_tracer_write_file(tracer,
            "instances/spf/trace_clock", "mono_raw");

    spf_tracer_write_file(tracer,
            "kprobe_events", "p:spf_eldu sgx_eldu addr=+0(%si)");

    spf_tracer_append_file(tracer,
            "kprobe_events", "p:spf_ewb sgx_ewb addr=+0(%si)");

    spf_tracer_write_file(tracer,
            "instances/spf/events/kprobes/spf_eldu/enable", "1");

    spf_tracer_write_file(tracer,
            "instances/spf/events/kprobes/spf_ewb/enable", "1");

    rho_path_join(SPF_TRACE_ROOTDIR, "instances/spf/trace_pipe", pipe_path,
            sizeof(pipe_path));

    tracer->fd = open(pipe_path, O_RDONLY);
    if (tracer->fd == -1)
        rho_errno_die(errno, "open(\"%s\")", pipe_path);

    rho_fd_setnonblocking(tracer->fd);

    spf_tracer_runloop(tracer);

    RHO_TRACE_EXIT();
    return;
}

void
spf_tracer_stop(struct spf_tracer *tracer)
{
    RHO_TRACE_ENTER();

    spf_tracer_write_file(tracer, "instances/spf/events/kprobes/spf_eldu/enable", "0");
    spf_tracer_write_file(tracer, "instances/spf/events/kprobes/spf_ewb/enable", "0");

    close(tracer->fd);
    spf_tracer_reset(tracer);

    rmdir("/sys/kernel/debug/tracing/instances/spf");
    
    RHO_TRACE_EXIT();
    return;
}

static void
spf_sigint_handler(int dummy)
{
    spf_keep_running = false;
}

int
main(int argc, char *argv[])
{
    struct spf_tracer *tracer = NULL;

    signal(SIGINT, spf_sigint_handler);

    tracer = spf_tracer_create();
    spf_tracer_start(tracer);
    /* runloop - CTL-C to exit */
    spf_tracer_stop(tracer);
    spf_tracer_destroy(tracer);

    return (0);
}
