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

spf depend on [librho](https://github.com/smherwig/librho).
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


<a name="running"/> Running
===========================

```
sudo ./spf
```
