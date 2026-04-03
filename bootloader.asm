; boot_fixed.asm
[BITS 16]
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7000           ; SAFER stack (avoid overwriting boot sector)
    mov [BOOT_DRIVE], dl

    ; --- print loading message (debug) ---
    mov si, msg_loading
.print_loading:
    lodsb
    or al, al
    jz .done_loading
    mov ah, 0x0E
    int 0x10
    jmp .print_loading
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
    mov bx, 0x1000        ; destination offset
    mov dl, [BOOT_DRIVE]
    xor dh, dh
    xor ch, ch  
    mov cl, 2                ; start sector = 2
    mov ah, 0x02             ; BIOS: read sectors
    int 0x13
    jc disk_error

    ; --- success debug print ---
    mov si, msg_loaded
    call print_string

    ; --- Setup GDT ---
    lgdt [gdt_descriptor]    ; gdt_descriptor in this same segment

    ; --- Enable protected mode ---
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; far jump to flush prefetch and load new CS
    ; after lgdt and setting CR0, instead of jumping directly to kernel:
    cli
    mov eax, cr0
    or eax, 1

    ; far jump into PM and run pm_test stub
    jmp 0x08:0x1000

; Replace your pm_test with this (after the far jump into 32-bit mode)
[BITS 32]
pm_test:
    ; load data selector (GDT selector 0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; safe stack (choose a real, writable location)
    mov esp, 0x90000

    ; write 'P' with attribute 0x0F (white on black) to VGA cell 0
    ; VGA memory at physical 0xB8000. Little-endian word needed: 0x0F50
    mov edi, 0xB8000
    mov word [edi], 0x0F50

    ; hang so you can visually inspect
.pm_hang:
    hlt
    jmp .pm_hang


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
gdt_start:
    dq 0x0000000000000000
    dq 0x00CF9A000000FFFF     ; code: base=0 limit=4GB
    dq 0x00CF92000000FFFF     ; data: base=0 limit=4GB
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

times 510 - ($ - $$) db 0
dw 0xAA55
