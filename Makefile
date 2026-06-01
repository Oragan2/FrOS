CROSS = x86_64-linux-gnu
AS = nasm
CC = $(CROSS)-gcc
LD = $(CROSS)-ld
PY = python3 FRFS.py files.bin

CFLAGS = -ffreestanding -m64 -mno-red-zone -mno-mmx -mno-sse -nostdlib -O2 -fno-pic
LDFLAGS = -T linker.ld -m elf_x86_64 -Map=kernel.map

QEMU = qemu-system-x86_64
QEMU_BASE = -drive format=raw,file=os.img -boot c -nodefaults -vga std -smp 1

all: os.img

os.img: bootloader.bin kernel.bin files.bin
	dd if=/dev/zero of=os.img bs=512 count=2048
	dd if=bootloader.bin of=os.img conv=notrunc
	dd if=kernel.bin of=os.img bs=512 seek=1 conv=notrunc,sync
	dd if=files.bin of=os.img bs=512 seek=50 conv=notrunc

# Automatically generate kernel size constant
bootloader_constants.asm: kernel.bin
	echo "kernel_size dw `stat -c %s kernel.bin`" > $@

bootloader.bin: bootloader.asm bootloader_constants.asm
	$(AS) -f bin $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

# 1. Update the dependency list for kernel.bin
kernel.bin: kernel_entry.o kernel.o string.o screen.o cmd.o inputs.o output.o disk.o mem.o fs.o pci.o pit.o idt.o switch.o idt_asm.o irq_stubs.o
	$(LD) $(LDFLAGS) -o $@ $^
	
# 2. Add the compilation rules for the new source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) -f elf64 $< -o $@

# 3 Add files to the mix
files.bin : test.txt ts.x bootloader.asm
	$(PY) $^

run:
	$(QEMU) $(QEMU_BASE) \
	  -device qemu-xhci,id=usb -device usb-kbd -device usb-tablet \
	  -display gtk -d int,cpu_reset -D qemu.log

clean:
	rm -rf *.o *.bin os.img
