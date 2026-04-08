; boot_fixed.asm
[BITS 16]
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; SAFER stack (avoid overwriting boot sector)
    mov [BOOT_DRIVE], dl

    ; --- print loading message (debug) ---
    mov si, msg_loading
    call print_string

.done_loading:
    ; -------- compute sectors to read ----------
    ; Assume kernel_size is a WORD (bytes). If it's a DWORD, use the 32-bit path below.
    mov ax, [kernel_size]    ; kernel size in bytes (word)
    xor dx, dx
    mov bx, 512
    div bx                   ; DX:AX / BX -> AX = sectors, DX = remainder
    cmp dx, 0
    je .no_inc
    inc ax
.no_inc:
    ; AX now contains number_of_sectors (word). We must place it into AL for int13.
    mov bl, al               ; preserve low 8 bits
    ; check overflow: if AX > 0xFF we'll fail because AL cannot exceed 255
    cmp ax, 0x00FF
    jbe .ok_sectors
    ; handle too large (print error and hang)
    mov si, msg_too_big
    call print_string
    jmp disk_error.hang
.ok_sectors:
    mov al, bl               ; AL = sectors count
    ; -------- BIOS read into ES:BX (dest = 0x0000:0x7E00) ----------
    mov al, 32
    mov ah, 0x02

    xor cx, cx
    mov es, cx
    mov bx, 0x7E00
    mov dl, [BOOT_DRIVE]
    xor dh, dh
    xor ch, ch  
    mov cl, 2                ; start sector = 2
    int 0x13
    jc disk_error
        
    in al, 0x92
    or al, 2
    out 0x92, al
    cli
    ; --- Setup GDT ---
    lgdt [gdt_descriptor]    ; gdt_descriptor in this same segment
    
    ; --- success debug print ---
    mov si, msg_loaded
    call print_string

    ; --- Enable protected mode ---
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; far jump into PM and run pm_test stub
    jmp dword 0x08:0x7E00

; Replace your pm_test with this (after the far jump into 32-bit mode)
[BITS 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    jmp 0x7E00
    jmp $


; ============================
; error handlers & helpers
; ============================
disk_error:
    mov si, msg_error
    call print_string
    jmp .hang

.hang:
    hlt
    jmp .hang

; simple string print (uses int 10 teletype)
print_string:
    pusha
    .1:
        lodsb
        or al, al
        jz .2
        mov ah, 0x0E
        int 0x10
        jmp .1
    .2:
    popa
    ret

; ============================
; GDT (flat segments)
; ============================
[BITS 16]
align 4
gdt_start:
    ; Null descriptor
    dq 0x0
; Code segment descriptor: base=0, limit=0xfffff, type=0x9a, flags=0xc
gdt_code:
    dw 0xffff    ; Limit (bits 0-15)
    dw 0x0  ; Base (bits 0-15)
    db 0x0      ; Base (bits 16-23)
    db 10011010b ; Access (Present, Ring 0, Code, Executable, Readable)
    db 11001111b ; Flags (4KB granularity, 32-bit) + Limit (bits 16-19)
    db 0x00      ; Base (bits 24-31)
; Data segment descriptor: base=0, limit=0xfffff, type=0x92, flags=0xc
gdt_data:
    dw 0xffff    ; Limit
    dw 0x0    ; Base
    db 0x0      ; Base
    db 10010010b ; Access (Present, Ring 0, Data, Writable)
    db 11001111b ; Flags
    db 0x00      ; Base
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
; ============================
; data
; ============================
BOOT_DRIVE  db 0

%include "bootloader_constants.asm"

msg_loading db "Loading kernel...", 0
msg_loaded  db 0x0D,0x0A, "Kernel read OK.",0
msg_error   db 0x0D,0x0A, "Disk read error!", 0
msg_too_big db 0x0D,0x0A, "Kernel too large for single read",0
debug db 0x0D,0x0A, "DEBUG",0

times 510 - ($ - $$) db 0
dw 0xAA55
