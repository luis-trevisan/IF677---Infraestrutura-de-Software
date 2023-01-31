org 0x7c00
jmp 0x0000:start

data:
  m: times 100 db 0

putchar:			;função pra printar o caractere
  mov ah, 0x0e
  int 10h
  ret
  
getchar:			;função pra pegar caractere do teclado
  mov ah, 0x00
  int 16h
  ret

delchar:
  mov al, 0x08		;backspace
  call putchar
  mov al, ' '
  call putchar
  mov al, 0x08
  call putchar
  ret
  
endl:
  mov al, 0x0a		;line feed
  call putchar
  mov al, 0x0d    ;carriage return
  call putchar
  ret

gets:				      ;função pra pegar e armazenar string
  xor cx, cx      ;zerar contador
  .loop1:
    call getchar
    cmp al, 0x08    ;backspace
    je .backspace
    cmp al, 0x0d    ;carriage return
    je .done
    cmp cl, 10      ;string limit checker
    je .loop1
    
    stosb
    inc cl
    call putchar
    
    jmp .loop1
    .backspace:
      cmp cl, 0       ;is empty?
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

  stoi:							; mov si, string
	xor cx, cx
	xor ax, ax
	.loop1:
		push ax
		lodsb
		mov cl, al
		pop ax
		cmp cl, 0				; check EOF(NULL)
		je .endloop1
		sub cl, 48				; '9'-'0' = 9
		mov bx, 10
		mul bx					; 999*10 = 9990
		add ax, cx				; 9990+9 = 9999
		jmp .loop1
	.endloop1:
	ret

totransform: 
    ;transformar m em numero  
    mov si, m
    call stoi
    push ax
    
    pop ax ;vai ser m
    
    ret
prints:              ; mov si, string
  lodsb          ; bota character em al 
  call putchar
  ret

start:
	xor ax, ax
	xor bx, bx
    
	mov ah, 00h        ;set video mode
	mov al, 00h
	int 10h
	call gets

  mov di, m
  call gets
  call totransform
  sub ax, 1

  mov si, ax
  call prints

times 510 - ($ - $$) db 0
dw 0xaa55