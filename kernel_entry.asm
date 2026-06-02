[BITS 32]
section .entry

global _start
_start:
    ; 1. Fix up 32-bit data segments right upon landing from bootloader
    cli
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000

    ; 2. Build identity page tables dynamically (Safe at 0x1000)
    mov edi, 0x1000
    xor eax, eax
    mov ecx, 3072
    rep stosd

    mov dword [0x1000], 0x2003
    mov dword [0x2000], 0x3003
    mov dword [0x3000], 0x4003

    mov edi, 0x4000
    mov eax, 0x00000003
.loop_page_table:
    mov [edi], eax
    add eax, 0x1000
    add edi, 8
    cmp eax, 0x200000
    jl .loop_page_table

    ; 3. Enable PAE
    mov eax, 0x1000
    mov cr3, eax
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 4. Turn on Long Mode bit in EFER MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 5. Turn on Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 6. Jump into 64-Bit execution space within this same file!
    jmp 0x18:init_lm

[BITS 64]
init_lm:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    mov rsp, 0x90000
    and rsp, 0xFFFFFFFFFFFFFFF0

    ; 7. Hand off to your true 64-bit C kernel_main function
    extern kernel_main
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
