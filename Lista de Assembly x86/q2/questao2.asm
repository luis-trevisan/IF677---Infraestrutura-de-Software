org 0x7c00
jmp 0x0000:start

putchar:			;função pra printar o caractere
  mov ah, 0x0e
  int 10h
  ret
  
getchar:			;função pra pegar caractere do teclado
  mov ah, 0x00
  int 16h
  ret

;AUXILIARES ---------------
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
;--------------------------

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
  
reverse:
  mov di, si
  xor cx, cx          ;zera contador
  .loop1:             ;bota string na stack
    lodsb
    cmp al, 0
    je .endloop1
    inc cl
    push ax
    jmp .loop1
  .endloop1:
  .loop2:             ;remover string da stack        
    pop ax
    stosb
    call putchar      ;printa o caractere
    loop .loop2
  ret


start:
	xor ax, ax
	xor bx, bx
    
	mov ah, 00h        ;set video mode
	mov al, 00h
	int 10h
	
	call gets          ;pega a string do teclado
	call reverse	     ;"inverte" e printa carcatere por caractere 

times 510 - ($ - $$) db 0
dw 0xaa55