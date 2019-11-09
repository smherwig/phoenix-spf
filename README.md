Overview
========

spf is SGX page fault measurement tool, and based on
[sgx-perf](https://github.com/ibr-ds/sgx-perf).  The tool uses kprobes to
record when the Intel SGX Linux kernel driver invokes the `sgx_eldu` and
`sgx_ewb` functions, which, in turn, invoke the SGX instructions `ELDU` and
`EWB`, respectively.  The SGX instruction `ELDU` loads a page from regular
memory into enclave memory (EPC), whereas `EWB` pages out a page from the EPC
to regular memory.


<a name="building"/> Building and Installing
============================================

spf depends on [librho](https://github.com/smherwig/librho).  I assume that
librho is installed under `$HOME`; modify spf's Makefile if this is not the
case.


To download and build spf, enter:

```
cd ~/src
git clone https:///github.com/smherwig/phoenix-spf spf
cd spf
make
```

To install, enter:

```
make install
```

By default, spf is installed to `/usr/local/`.
To install to a different, location, say, `$HOME`, enter

```
make install INSTALL_TOP=$HOME
```


<a name="running"/> Running
===========================

```
sudo ./spf
```

To stop, hit `CTRL-C`.


`spf` writes the events to stdout; example output lines are:

```
<...>-2105  [006] d...  6739.020106: spf_ewb: (sgx_ewb+0x0/0xd2 [isgx]) addr=0x5e8c000
<...>-13563 [005] d...  6739.020221: spf_eldu: (sgx_eldu+0x0/0x35e [isgx]) addr=0x7ffa000
```
