# ackOS Rewrite To-Do

## Stage #0 [DONE]
* [x] Setup build system
* [x] Boot into a higher half 64 bit environment (using limine)
* [x] Write a "Hello World" kernel

## Stage #1 [DONE]
* [x] Load a GDT
* [x] Load a IDT and configure interrupts
* [x] Read memory map from bootloader and initalise memory
* [x] Page mapping

## Stage #2
* [ ] Write unit tests
* [x] Fix issue [#2](https://github.com/ackOS-project/ackOS/issues/2) and make a better logo
* [ ] Read ACPI tables and setup APIC
* [ ] Setup system for SMP

## Stage #3
* [ ] Implement the virtual filesystem
* [ ] Write USB keyboard drivers (because PS/2 is dead)
* [ ] Enter userspace


