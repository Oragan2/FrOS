[org 0x7c00]
[bits 16]

start:
    cli                         ; Disable interrupts during setup
    xor ax, ax                  ; Segment 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00              ; Stack grows downwards from 0x7c00

    ; Save the boot drive number BIOS gave us in DL
    mov [BOOT_DRIVE], dl        

    mov si, msg_loading
    call print_string

    ; Set up the LBA disk packet with the Makefile's sector count
    mov si, disk_packet
    mov ax, KERNEL_SECTORS
    mov [disk_packet.sector_count], ax 

    mov ah, 0x42                ; BIOS LBA Read function
    mov dl, [BOOT_DRIVE]        
    int 0x13
    jc disk_error               ; If carry flag is set, disk read failed

    mov si, msg_loaded
    call print_string

    ; =========================================================================
    ; BRIDGE TO KERNEL_ENTRY: Transition to 32-bit Protected Mode
    ; =========================================================================
    cli                         ; 1. Clear interrupts permanently for protected mode
    
    ; 2. Enable the A20 Line (Fast A20 method via keyboard controller port)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; 3. Load the Global Descriptor Table (GDT)
    lgdt [gdt_pointer]          

    ; 4. Flip Bit 0 of CR0 to turn on Protected Mode hardware profiling
    mov eax, cr0
    or eax, 1                   
    mov cr0, eax

    ; 5. Execute far jump into 32-bit space! 
    ; 0x08 is our Code Segment Selector. 0x7E00 is where the kernel was loaded.
    jmp 0x08:0x7E00             

; =========================================================================
; Error Handlers & Support Functions
; =========================================================================
disk_error:
    mov si, msg_disk_err
    call print_string
    cli
.hang:
    hlt
    jmp .hang

print_string:
    pusha
.loop:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0e
    int 0x10
    jmp .loop
.done:
    popa
    ret

; =========================================================================
; Data Structures & Global Descriptor Table (GDT)
; =========================================================================
BOOT_DRIVE:   db 0
msg_loading:  db "Loading Kernel...", 13, 10, 0
msg_loaded:   db "Kernel Loaded. Switching modes...", 13, 10, 0
msg_disk_err: db "Disk Read Error!", 13, 10, 0

align 4
disk_packet:
    db 0x10                     ; Packet size (16 bytes)
    db 0x00                     ; Reserved
.sector_count:
    dw 0                        ; Sector count (Filled at runtime)
    dw 0x0000                   ; Destination Offset
    dw 0x07E0                   ; Destination Segment (0x07E0:0000 = 0x7E00 physical)
    dq 1                        ; Read starting from LBA Sector 1 (the kernel)

; --- The GDT Table Layout ---
; Your kernel_entry relies on Data being 0x10 and Long Mode Code being 0x18
align 8
gdt_start:
    dq 0x0000000000000000       ; Null Descriptor (Selector 0x00)

gdt_code32:                     ; 32-Bit Code Descriptor (Selector 0x08)
    dw 0xffff, 0x0000
    db 0x00, 0x9a, 0xcf, 0x00

gdt_data32:                     ; 32-Bit Data Descriptor (Selector 0x10)
    dw 0xffff, 0x0000
    db 0x00, 0x92, 0xcf, 0x00

gdt_code64:                     ; 64-Bit Long Mode Code Descriptor (Selector 0x18)
    dw 0x0000, 0x0000
    db 0x00, 0x9a, 0x20, 0x00
gdt_end:

gdt_pointer:
    dw gdt_end - gdt_start - 1  ; Size of GDT table minus 1
    dd gdt_start                ; Linear address of table

%include "bootloader_constants.asm" ; Pulls in KERNEL_SECTORS from Makefile

; Magic Boot Signature Padding
times 510-($-$$) db 0
dw 0xAA55
