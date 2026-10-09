.section .multiboot
.align 8

header_start:

.long 0xE85250D6
.long 0
.long header_end - header_start
.long -(0xE85250D6 + 0 + (header_end - header_start))

.align 8
.short 0
.short 0
.long 8

header_end:


.section .text
.code32

.global _start
.type _start, @function

_start:
    cli

    # Preserve GRUB's Multiboot2 magic and information pointer.
    mov %eax, multiboot_magic
    mov %ebx, multiboot_info_addr

    mov $stack_top, %esp

    # PML4[0] -> PDPT
    mov $pdpt_table, %eax
    or $0x3, %eax
    mov %eax, pml4_table

    # PDPT[0] -> page directory
    mov $pd_table, %eax
    or $0x3, %eax
    mov %eax, pdpt_table

    # Identity-map first 1 GiB using 2 MiB pages.
    mov $pd_table, %edi
    mov $0x00000083, %eax
    mov $512, %ecx

.map_pages:
    mov %eax, (%edi)
    add $0x200000, %eax
    add $8, %edi
    loop .map_pages

    # Load GDT.
    lgdt gdt64_pointer

    # Enable PAE.
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    # Load PML4.
    mov $pml4_table, %eax
    mov %eax, %cr3

    # Enable Long Mode.
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr

    # Enable paging.
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    ljmp $0x08, $long_mode_start


.code64

long_mode_start:

    # Load 64-bit data segments.
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %ss

    # Set the 64-bit stack.
    mov $stack_top, %rsp

    # kernel_main(uint32_t magic, uint32_t mbi_addr)
    # System V AMD64: first argument RDI, second argument RSI.
    movl multiboot_magic(%rip), %edi
    movl multiboot_info_addr(%rip), %esi

    call kernel_main
.hang:
    cli
    hlt
    jmp .hang


.section .rodata
.align 8

gdt64:
    # Null descriptor
    .quad 0x0000000000000000

    # 64-bit code segment
    .quad 0x00AF9A000000FFFF

    # Data segment
    .quad 0x00AF92000000FFFF

gdt64_pointer:
    .word gdt64_pointer - gdt64 - 1
    .long gdt64


.section .bss
.align 4
multiboot_magic:
    .long 0

multiboot_info_addr:
    .long 0
.align 4096

# PML4
pml4_table:
    .skip 4096

# Page directory pointer table
pdpt_table:
    .skip 4096

# Page directory
pd_table:
    .skip 4096

# Kernel stack
stack_bottom:
    .skip 16384
stack_top: