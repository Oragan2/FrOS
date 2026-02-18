CROSS = i686-elf
AS = nasm
CC = $(CROSS)-gcc
LD = $(CROSS)-ld
PY = python3 FRFS.py files.bin

CFLAGS = -ffreestanding -m32 -nostdlib -O2
LDFLAGS = -Ttext 0x1000 --oformat binary -m elf_i386 -e kernel_entry

all: os.img

os.img: bootloader.bin kernel.bin files.bin
	# 1. Create a blank 1MB image (adjust size as needed)
	dd if=/dev/zero of=os.img bs=512 count=2048
	# 2. Place the bootloader at the very start
	dd if=bootloader.bin of=os.img conv=notrunc
	# 3. Place the kernel at LBA 1 (byte 512)
	dd if=kernel.bin of=os.img bs=512 seek=1 conv=notrunc
	# 4. Place your FS at a FIXED LBA (e.g., LBA 50)
	dd if=files.bin of=os.img bs=512 seek=50 conv=notrunc

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
kernel.bin: kernel_entry.o kernel.o string.o screen.o cmd.o inputs.o output.o disk.o mem.o fs.o
	$(LD) $(LDFLAGS) -o $@ $^
	
# 2. Add the compilation rules for the new source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 3 Add files to the mix
files.bin : test.txt ts.x bootloader.asm
	$(PY) $^

# 4. Update the clean target to remove new files
clean:
	rm -f *.bin *.o os.img bootloader_constants.asm

run:
	qemu-system-i386 -drive format=raw,file=os.img -boot a
