DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export PATH=$DIR/toolchain/local/bin/:$PATH
YELLOW='\033[1;33m'
ISONAME='ackos'

tput setaf 1; nasm -f elf64 kernel-core/boot/header.asm -o boot.o
nasm -f elf64 kernel-core/boot/header.asm -o boot.o

if [ $? -ne 0 ]
then
    echo -e "${YELLOW}The assembler did not succeed"
    exit 1
fi
tput sgr0;
x86_64-elf-gcc -c kernel.cpp -o kernel.o -ffreestanding -m64 -MD -std=c++17 -v -O2 -fPIC -fno-exceptions -fno-rtti
if [ $? -ne 0 ]
then
    echo -e "${YELLOW}The compiler did not succeed"
    exit 1
fi
# tput setaf 1;
objcopy -O elf64-x86-64 -B i386 -I binary zap-vga16.psf font.o
x86_64-elf-ld -n -o $ISONAME.bin -T linker.ld boot.o kernel.o font.o -O2 -nostdlib
if [ $? -ne 0 ]
then
    echo -e "${YELLOW}The linker did not succeed"
    exit 1
fi
printf '\e[m'
cp $ISONAME.bin isodir/boot/$ISONAME.bin
cp grub.cfg isodir/boot/grub/grub.cfg

if grub-file --is-x86-multiboot2 $ISONAME.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi

sudo grub-mkrescue -o $ISONAME.iso isodir
echo -e "${YELLOW}ISO was written to ${ISONAME}.iso"
qemu-system-x86_64 -serial stdio -cdrom $ISONAME.iso
