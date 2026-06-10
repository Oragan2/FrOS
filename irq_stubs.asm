[BITS 64]

extern pit_handler
global irq0_stub
irq0_stub:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    call pit_handler
    
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax

    iretq

global exception_handler
exception_handler:
    mov al, 'E'          ; Display 'E' for Exception
    mov ah, 0x4F         ; Red background, White text
    mov [0xB8000], ax    ; Print to top-left of screen
    jmp $
