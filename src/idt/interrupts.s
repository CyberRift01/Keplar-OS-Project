.intel_syntax noprefix

.section .text
.code64

.global exception_handler
.global irq0_stub
.global irq1_stub
.global keyboard_isr

.type exception_handler, @function
.type irq0_stub, @function
.type irq1_stub, @function
.type keyboard_isr, @function

.extern terminal_write
.extern terminal_putchar
.extern keyboard_getchar
.extern pic_send_eoi
.extern irq0_handler
.extern irq1_handler


# --------------------------------------------------
# CPU exception handler
# NOTE: This is only a basic halt handler, not a
# universal handler for every exception type.
# --------------------------------------------------

# --------------------------------------------------
# exception handler
# --------------------------------------------------

exception_handler:
    cli

    lea rdi, [rip + exception_message]

    # Align the stack before calling C.
    and rsp, -16
    call terminal_write

.exception_halt:
    hlt
    jmp .exception_halt


# --------------------------------------------------
# IRQ0 — PIT timer
# irq0_handler() is responsible for sending EOI.
# --------------------------------------------------

irq0_stub:
    cld

    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r12, rsp
    and rsp, -16
    call irq0_handler
    mov rsp, r12

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq


# --------------------------------------------------
# IRQ1 — Keyboard via C handler
# irq1_handler() is responsible for sending EOI.
# --------------------------------------------------

irq1_stub:
    cld

    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r12, rsp
    and rsp, -16
    call irq1_handler
    mov rsp, r12

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq


# --------------------------------------------------
# Keyboard ISR — reads a character and prints it
# Assumes keyboard_getchar() returns a character
# in AL, or zero when there is no character.
# --------------------------------------------------

keyboard_isr:
    cld

    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r12, rsp
    and rsp, -16

    # Read the keyboard character.
    call keyboard_getchar

    test al, al
    jz .keyboard_no_character

    # terminal_putchar(character)
    movzx edi, al
    call terminal_putchar

.keyboard_no_character:
    # Acknowledge IRQ1 at the PIC.
    mov edi, 1
    call pic_send_eoi

    mov rsp, r12

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq


.section .rodata

exception_message:
    .asciz "\nKeplar: CPU exception!\n"


.section .note.GNU-stack,"",@progbits
