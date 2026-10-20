
.intel_syntax noprefix

.section .text
.code64

.global exception_handler
.extern irq_dispatch_handler

# Basic CPU exception handler.
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
# dispatch the IRQ through C, restore registers,
# and return from the interrupt.
# --------------------------------------------------

.macro IRQ_STUB name, number
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

    # Preserve the saved-register stack pointer.
    mov r12, rsp

    # Align stack for the C ABI call.
    and rsp, -16

    # First argument: IRQ number.
    mov edi, \number

    call irq_dispatch_handler

    # Restore the original saved-register stack.
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


# All IRQs use the common dispatcher.
IRQ_STUB irq0_stub,  0
IRQ_STUB irq1_stub,  1
IRQ_STUB irq2_stub,  2
IRQ_STUB irq3_stub,  3
IRQ_STUB irq4_stub,  4
IRQ_STUB irq5_stub,  5
IRQ_STUB irq6_stub,  6
IRQ_STUB irq7_stub,  7
IRQ_STUB irq8_stub,  8
IRQ_STUB irq9_stub,  9
IRQ_STUB irq10_stub, 10
IRQ_STUB irq11_stub, 11
IRQ_STUB irq12_stub, 12
IRQ_STUB irq13_stub, 13
IRQ_STUB irq14_stub, 14
IRQ_STUB irq15_stub, 15

.section .note.GNU-stack,"",@progbits