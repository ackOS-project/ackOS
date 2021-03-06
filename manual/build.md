# Building ackOS

The **cross compiler toolchain** must be built first

## Prerequisites
If you're on ubuntu or an ubuntu based distro, such as Linux Mint type this
```
sudo apt-get install build-essential nasm gcc g++ bison flex xoriso qemu-system-x86 libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```
Sorry Arch users, I don't have the time to test on Arch linux. Try using virtual machine or a docker container

## Building the toolchain
Simply run this in the root of ackOS
```
$ ./toolchain/build.sh
```
This should take while so be patient

# Build ackOS
To build and run ackOS run
```
$ make all
$ make qemu
```

Have fun :)