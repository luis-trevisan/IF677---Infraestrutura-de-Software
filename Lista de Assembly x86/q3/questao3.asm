org 0x7c00
jmp 0x0000:start

data:
    string times 10 db 0

    amarelo db 'amarelo', 0
    azul db 'azul', 0
    verde db 'verde', 0
    vermelho db 'vermelho', 0

    AMARELO db 'AMARELO', 0
    AZUL db 'AZUL', 0
    VERDE db 'VERDE', 0
    VERMELHO db 'VERMELHO', 0
    NAO_EXISTE db 'NAO EXISTE', 0

getchar:
    mov ah, 0x00
    int 16h
    ret

putchar:
    mov ah, 0x0e
    int 10h
    ret

delchar:
  mov al, 0x08
  call putchar

  mov al, ' '
  call putchar

  mov al, 0x08
  call putchar

  ret

endl:
  mov al, 0x0a
  call putchar

  mov al, 0x0d
  call putchar

  ret


gets:                 ; mov di, string
    xor cx, cx          ; zerar contador

    .loop1:
        call getchar
        cmp al, 0x08      ; backspace
        je .backspace
        cmp al, 0x0d      ; carriage return
        je .done
        cmp cl, 10        ; string limit checker
        je .loop1
    
    stosb
    inc cl
    call putchar
    jmp .loop1
    
    .backspace:
        cmp cl, 0       ; is empty?
        je .loop1
        dec di
        dec cl
        mov byte[di], 0
        call delchar
        jmp .loop1

    .done:
        mov al, 0
        stosb
        call endl
        ret

strcmp:
    .loop1:
        lodsb
        cmp al, byte[di]
        jne .notequal
        cmp al, 0
        je .equal
        inc di
        jmp .loop1
    .notequal:
        clc
        ret
    .equal:
        stc
        ret

prints:
    .loop:
        lodsb
        cmp al, 0
        je .endloop

        call putchar
        jmp .loop
    
    .endloop:
    ret

start:
    xor ax, ax
    
    mov ah, 00h
    int 10h
    mov di, string
    call gets

    mov si, string
    mov di, amarelo
    call strcmp
    je .amarelo

    mov si, string
    mov di, azul
    call strcmp
    je .azul

    mov si, string
    mov di, verde
    call strcmp
    je .verde

    mov si, string
    mov di, vermelho
    call strcmp
    je .vermelho

    mov si, NAO_EXISTE
    mov bl, 5
    jmp .fim

    .amarelo:
        mov si, AMARELO
        mov bl, 14
        jmp .fim
    .azul:
        mov si, AZUL
        mov bl, 1
        jmp .fim
    .verde:
        mov si, VERDE
        mov bl, 2
        jmp .fim
    .vermelho:
        mov si, VERMELHO
        mov bl, 4

    .fim:
        mov ax, 13h
        int 10h
        call prints

times 510 - ($ - $$) db 0
dw 0xaa55