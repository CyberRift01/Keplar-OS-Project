
.intel_syntax noprefix

.section .text
.code64

.global exception_handler

.extern irq0_handler
.extern irq1_handler
.extern irq_dispatch_handler

# Basic CPU exception handler
.type exception_handler, @function
exception_handler:
    cli
.exception_halt:
    hlt
    jmp .exception_halt


# --------------------------------------------------
# IRQ stub macro
#
# Save general-purpose registers, align the stack,
# call the appropriate C handler, restore registers,
# then return from the interrupt.
# --------------------------------------------------

.macro IRQ_STUB name, number, handler
.global \name
.type \name, @function

\name:
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

    .if \number >= 2
        mov edi, \number
    .endif

    call \handler

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
.endm


# IRQ0 and IRQ1 retain their existing C handlers.
IRQ_STUB irq0_stub, 0, irq0_handler
IRQ_STUB irq1_stub, 1, irq1_handler

# Remaining legacy PIC IRQs use the common dispatcher.
IRQ_STUB irq2_stub,  2, irq_dispatch_handler
IRQ_STUB irq3_stub,  3, irq_dispatch_handler
IRQ_STUB irq4_stub,  4, irq_dispatch_handler
IRQ_STUB irq5_stub,  5, irq_dispatch_handler
IRQ_STUB irq6_stub,  6, irq_dispatch_handler
IRQ_STUB irq7_stub,  7, irq_dispatch_handler
IRQ_STUB irq8_stub,  8, irq_dispatch_handler
IRQ_STUB irq9_stub,  9, irq_dispatch_handler
IRQ_STUB irq10_stub, 10, irq_dispatch_handler
IRQ_STUB irq11_stub, 11, irq_dispatch_handler
IRQ_STUB irq12_stub, 12, irq_dispatch_handler
IRQ_STUB irq13_stub, 13, irq_dispatch_handler
IRQ_STUB irq14_stub, 14, irq_dispatch_handler
IRQ_STUB irq15_stub, 15, irq_dispatch_handler

.section .note.GNU-stack,"",@progbits