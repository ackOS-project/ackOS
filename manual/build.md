# Building ackOS

## Prerequisites
If you're on Ubuntu or any Ubuntu based distro, such as Linux Mint type this
```
sudo apt install build-essential nasm gcc g++ bison flex xorriso qemu-system-x86 libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```
Arch users: Sorry but I'm not a btwOS user so don't think I can help you here.

## Fetch the submodules
*This step is IMPORTANT*

Make sure to clone with `--recursive` to fetch all the submodules. Like this:
```
git clone https://github.com/ackOS-project/ackOS.git --recursive
```

## Building the toolchain
Simply run this in the root of ackOS
```
$ ./toolchain/build.sh
```
This will take a while so be patient.

# Build ackOS
**IMPORTANT**: The **cross compiler toolchain** must be built first before continuing with the build instructions.

To build ackOS run
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

If you don't already have Bochs installed you can install it (on Ubuntu) with

```
$ sudo apt install bochs bochs-x bochsbios vgabios
```

## Notes on Debugging
There are several ways you can debug ackOS.

You can start a debugging instance of qemu with:
```
make qemu-debug
```
and start gdb with 
```
make gdb
```
or you could select the 'Attach to QEMU with GDB' option from the VSCode debug sidebar.

ackOS also creates an log file called 'ackos.log' located in the root directory, which may contain some useful debugging information.

Have fun :)
