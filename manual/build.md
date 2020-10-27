# Building ackOS

The **cross compiler toolchain** must be built first

## Prerequisites
If you're on ubuntu or an ubuntu based distro, such as Linux Mint type this
```
sudo apt-get install build-essential gcc g++ bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```
Sorry arch users, I don't have access to an arch machine. If you do know the required packages for arch, opening a pull request would be welcomed

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