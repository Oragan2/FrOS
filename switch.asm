[BITS 32]
save_context:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    ret

global switch_context
switch_context:
    push ebp
    push ebx
    push esi
    push edi

    mov eax, [esp+20]
    mov edx, [esp+24]

    mov [eax], esp
    mov esp, [edx]

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
