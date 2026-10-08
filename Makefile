CC = gcc
AS = as
LD = ld

CFLAGS = -std=c11 -ffreestanding -O2 -Wall -Wextra \
         -m64 -mno-red-zone -fno-pie -fno-stack-protector \
         -Isrc/main -Isrc/idt -Isrc/terminal -Isrc/shell

LDFLAGS = -T linker.ld -m elf_x86_64

all: keplar.iso

boot.o: boot/boot.s
	$(AS) --64 $< -o $@

keyboard.o: src/main/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

shell.o: src/shell/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

terminal.o: src/terminal/terminal.c
	$(CC) $(CFLAGS) -c $< -o $@

idt.o: src/idt/idt.c
	$(CC) $(CFLAGS) -c $< -o $@

interrupts.o: src/idt/interrupts.s
	$(AS) --64 $< -o $@

kernel.o: src/main/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

keplar.bin: boot.o kernel.o terminal.o keyboard.o shell.o linker.ld idt.o interrupts.o
	$(LD) $(LDFLAGS) \
		boot.o \
		kernel.o \
		terminal.o \
		keyboard.o \
		shell.o \
		idt.o \
		interrupts.o \
		-o $@

keplar.iso: keplar.bin grub.cfg
	rm -rf iso
	mkdir -p iso/boot/grub
	cp keplar.bin iso/boot/
	cp grub.cfg iso/boot/grub/
	grub-mkrescue -o keplar.iso iso

clean:
	rm -rf *.o *.bin *.iso iso