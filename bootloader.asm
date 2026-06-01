[BITS 16]
ORG 0x7C00

start:
    mov [BOOT_DRIVE], dl

    mov eax, 1
    cpuid
    shr ebx, 24
    cmp ebx, 0
    je .primary_core

.ap_loop:
    cli
    hlt
    jmp .ap_loop

.primary_core:
    jmp 0x0000:.flush_cs

.flush_cs:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00          ; SAFER stack (avoid overwriting boot sector)

    ; --- print loading message (debug) ---
    mov si, msg_loading
    call print_string

.done_loading:
    ; -------- compute sectors to read ----------
    mov ax, [kernel_size]    ; kernel size in bytes (word)
    xor dx, dx
    mov bx, 512
    div bx                   ; DX:AX / BX -> AX = sectors, DX = remainder
    cmp dx, 0
    je .no_inc
    inc ax
.no_inc:
    cmp ax, 0x00FF
    jbe .ok_sectors
    mov si, msg_too_big
    call print_string
    jmp disk_error.hang
.ok_sectors:
    mov cx, ax
    mov al, 1
    mov cl, 2

.read_loop:
    push cx

    mov ah, 0x02
    mov al, 1 
    mov dl, [BOOT_DRIVE]
    xor dh, dh

    int 0x13 
    jc disk_error

    mov ax, es
    add ax, 0x0020
    mov es, ax

    inc cl
    cmp cl, 18 
    jbe .next_sector
    mov cl, 1 
    inc dh 

.next_sector:
    pop cx
    loop .read_loop
        
    mov si, msg_loaded	
    call print_string
    
    in al, 0x92
    or al, 2
    out 0x92, al

    cli
    lgdt [gdt_descriptor]    ; gdt_descriptor in this same segment

    ; --- Enable protected mode ---
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; far jump into PM and run pm_test 
    jmp dword 0x08:pm_entry

; =====================================================================
; 32-BIT PROTECTED MODE SCRIPT ENTRY STUB
; =====================================================================
[BITS 32]
pm_entry:
    ; Instantly refresh all segment selectors to use our 32-bit Data Descriptor (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Update stack pointer to handle 32-bit execution boundaries
    mov esp, 0x90000             

    ; Jump directly to the loaded kernel's entry point address!
    jmp 0x7E00

[BITS 16]
disk_error:
    mov si, msg_error
    call print_string
.hang:
    hlt
    jmp .hang

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

align 4
gdt_start:
    dq 0x0
    
    dw 0xffff, 0x0
    db 0x0, 10011010b, 11001111b, 0x0
    
    dw 0xffff, 0x0
    db 0x0, 10010010b, 11001111b, 0x0

    dw 0x0000, 0x0000
    db 0x00, 10011010b, 00100000b, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

BOOT_DRIVE db 0
%include "bootloader_constants.asm"

msg_loading db "Loading kernel...", 0
msg_loaded db 0x0D,0x0A, "Kernel read OK", 0
msg_error db 0x0D,0x0A, "Disk read error", 0
msg_too_big db 0x0D,0x0A, "Kernel too large for single read", 0

times 510 - ($ - $$) db 0
dw 0xAA55
