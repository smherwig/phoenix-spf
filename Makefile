CFLAGS= -I$(HOME)/include -DRHO_TRACE -DHO_DEBUG -Wall -Werror

spf: spf.c
	$(CC) -o spf $(CFLAGS) spf.c $(HOME)/lib/librho.a

clean:
	rm -f spf

.PHONY: clean
