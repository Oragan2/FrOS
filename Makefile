CROSS = i686-elf
AS = nasm
CC = $(CROSS)-gcc
LD = $(CROSS)-ld

CFLAGS = -ffreestanding -m32 -nostdlib -O2
LDFLAGS = -Ttext 0x1000 --oformat binary -m elf_i386 -e kernel_entry

all: os.img

os.img: bootloader.bin kernel.bin
	cat bootloader.bin kernel.bin > os.img

# Automatically generate kernel size constant
bootloader_constants.asm: kernel.bin
	echo "kernel_size dw `stat -c %s kernel.bin`" > $@

bootloader.bin: bootloader.asm bootloader_constants.asm
	$(AS) -f bin $< -o $@

kernel_entry.o: kernel_entry.asm
	$(AS) -f elf32 $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

# 1. Update the dependency list for kernel.bin
kernel.bin: kernel_entry.o kernel.o string.o screen.o cmd.o
	$(LD) $(LDFLAGS) -o $@ kernel_entry.o kernel.o string.o screen.o cmd.o

# 2. Add the compilation rules for the new source files
string.o: string.c
	$(CC) $(CFLAGS) -c $< -o $@

screen.o: screen.c
	$(CC) $(CFLAGS) -c $< -o $@

cmd.o: cmd.c
	$(CC) $(CFLAGS) -c $< -o $@

# 3. Update the clean target to remove new files
clean:
	rm -f *.bin *.o os.img bootloader_constants.asm

run:
	qemu-system-i386 -drive format=raw,file=os.img -boot a
