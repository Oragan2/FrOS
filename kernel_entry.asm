[BITS 32]

section .entry

global _start
_start:
    db 0xB8, 0x10, 0x00, 0x00, 0x00
    mov ds, ax
    mov es, ax     ; <--- ADDED: Set Extra Segment
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000         ; safer stack below 1MiB

    and esp, 0xFFFFFFF0     ; Zero out the last 4 bits of the stack pointer

    ; Debug: write 'K' to VGA cell 0 (white on black)
    mov edi, 0xB8000
    mov word [edi], 0x0F4B  ; 0x0F << 8 | 'K'(0x4B)
    
    extern kernel_main
    call kernel_main
    jmp $

.hang:
    cli
    hlt
    jmp .hang
