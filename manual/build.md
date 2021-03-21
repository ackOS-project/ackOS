# Building ackOS

## Prerequisites
If you're on Ubuntu or an Ubuntu based distro, such as Linux Mint type this
```
sudo apt-get install build-essential nasm gcc g++ bison flex xoriso qemu-system-x86 libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```
Sorry Arch users, I don't have the time to test on Arch linux. Try using virtual machine or a docker container.

## Building the toolchain
Simply run this in the root of ackOS
```
$ ./toolchain/build.sh
```
This should take while so be patient

# Build ackOS
**IMPORTANT**: The **cross compiler toolchain** must be built first before continuing to compile ackOS

To build and run ackOS run
```
$ make all
```

## Running in QEMU
QEMU is prefered way of testing ackOS
```
$ make qemu
```

## Running in Bochs
Bochs is recommended if you need extra debugging information
```
$ make bochs
```

If you don't already have Bochs installed you can install with
(on Ubuntu)
```
$ sudo apt install bochs bochs-x bochsbios vgabios
```

Have fun :)