[BITS 32]

extern pit_handler
global irq0_stub
irq0_stub:
    pushad
    call pit_handler
    popad
    iretd

global exception_handler
exception_handler:
    mov al, 'E'          ; Display 'E' for Exception
    mov ah, 0x4F         ; Red background, White text
    mov [0xB8000], ax    ; Print to top-left of screen
    jmp $
