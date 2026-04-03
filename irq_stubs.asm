[BITS 32]

extern pit_handler
global irq0_stub
irq0_stub:
    pusha
    call pit_handler
    popa
    iret
