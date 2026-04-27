CROSS = i686-elf
AS = nasm
CC = $(CROSS)-gcc
LD = $(CROSS)-ld
PY = python3 FRFS.py files.bin

CFLAGS = -ffreestanding -m32 -nostdlib -O2
LDFLAGS = -T linker.ld -m elf_i386 -Map=kernel.map

QEMU = qemu-system-i386
QEMU_BASE = -drive format=raw,file=os.img -boot a -nodefaults -vga std

all: os.img

os.img: bootloader.bin kernel.bin files.bin
	dd if=/dev/zero of=os.img bs=512 count=2048
	dd if=bootloader.bin of=os.img conv=notrunc
	dd if=kernel.bin of=os.img bs=512 seek=1 conv=notrunc
	dd if=files.bin of=os.img bs=512 seek=50 conv=notrunc

# Automatically generate kernel size constant
bootloader_constants.asm: kernel.bin
	echo "kernel_size dw `stat -c %s kernel.bin`" > $@

bootloader.bin: bootloader.asm bootloader_constants.asm
	$(AS) -f bin $< -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

# 1. Update the dependency list for kernel.bin
kernel.bin: kernel_entry.o kernel.o string.o screen.o cmd.o inputs.o output.o disk.o mem.o fs.o pci.o pit.o idt.o switch.o idt_asm.o irq_stubs.o debug.o
	$(LD) $(LDFLAGS) -o $@ $^
	
# 2. Add the compilation rules for the new source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) -f elf32 $< -o $@

# 3 Add files to the mix
files.bin : test.txt ts.x bootloader.asm
	$(PY) $^

test-build:
	$(MAKE) CFLAGS="$(CFLAGS) -DTESTING" all

ci: test-build test

clean:
	rm -f *.bin *.o os.img bootloader_constants.asm

run:
	$(QEMU) $(QEMU_BASE) \
	  -device qemu-xhci,id=usb -device usb-kbd -device usb-tablet \
	  -display gtk

test:
	$(QEMU) $(QEMU_BASE) \
	  -display none \
	  -debugcon stdio \
	  -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
	  > boot.log 2>&1; \
	[ $$? -eq 1 ] || exit 1   # isa-debug-exit(0) → QEMU exits 1
	grep -q "\[OK\] BOOT COMPLETE" boot.log
