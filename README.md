Overview
========

spf is SGX page fault measurement tool, and based on
[sgx-perf](https://github.com/ibr-ds/sgx-perf).  The tool uses kprobes to
record when the Intel SGX Linux kernel driver invokes the `sgx_eldu` and
`sgx_ewb` functions, which invoke the SGX instructions `ELDU` and `EWB`,
respectively.  `ELDU` loads  page from regular memory into enclave memory
(EPC), whereas `EWB` pages out a page from the EPC to regular memory.


Building and Installing
=======================

spf depend on [librho](https://github.com/smherwig/librho).
[librpc](https://github.com/smherwig/phoenix-librpc).
I assume that librho is installed under `$HOME`; modify spf's
Makefile if this is not the case.


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


Running
=======

```
./spf
```
