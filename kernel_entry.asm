[BITS 32]
[GLOBAL kernel_entry]
[EXTERN kernel_main]

kernel_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax     ; <--- ADDED: Set Extra Segment
    mov fs, ax     ; <--- ADDED: Set FS Segment
    mov gs, ax     ; <--- ADDED: Set GS Segment
    mov ss, ax
    mov esp, 0x90000         ; safer stack below 1MiB

    and esp, 0xFFFFFFF0     ; Zero out the last 4 bits of the stack pointer

    ; Debug: write 'K' to VGA cell 0 (white on black)
    mov edi, 0xB8000
    mov word [edi], 0x0F4B  ; 0x0F << 8 | 'K'(0x4B)

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
