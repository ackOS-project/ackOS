export ARCH=arm
export PATH=/home/$USER/cross/elf64/bin/:$PATH
YELLOW='\033[1;33m'
ISONAME='ackos'

tput setaf 1; i686-elf-as boot.s -o boot.o
if [ $? -ne 0 ]
then
    echo -e "${YELLOW}The assembler did not succeed"
    exit 1
fi
tput setaf 1;
#i686-elf-gcc -c liballoc.c -ffreestanding -O2 -fno-exceptions
i686-elf-g++ -c kernel.cpp -o kernel.o -ffreestanding -O2 -fPIC -fno-exceptions -fno-rtti
if [ $? -ne 0 ]
then
    echo -e "${YELLOW}The compiler did not succeed"
    exit 1
fi
tput setaf 1;i686-elf-g++ -T linker.ld -o $ISONAME.bin -ffreestanding -O2 -nostdlib boot.o kernel.o
if [ $? -ne 0 ]
then
    echo -e "${YELLOW}The linker did not succeed"
    exit 1
fi
printf '\e[m'
cp $ISONAME.bin isodir/boot/$ISONAME.bin
cp grub.cfg isodir/boot/grub/grub.cfg

if grub-file --is-x86-multiboot $ISONAME.bin; then
  echo multiboot confirmed
else
  echo the file is not multiboot
fi

sudo grub-mkrescue -o $ISONAME.iso isodir
echo -e "${YELLOW}ISO was written to ${ISONAME}.iso"
qemu-system-x86_64 -cdrom $ISONAME.iso
