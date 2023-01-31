org 0x7c00
jmp 0x0000:start

data:
	string times 15 db 0
; calls
putchar:
	mov ah, 0x0e
	int 10h
	ret
getchar:
	mov ah, 0x00
	int 16h
	ret
delchar:
	mov al, 0x08					; backspace
	call putchar
	mov al, ' '
	call putchar
	mov al, 0x08					; backspace
	call putchar
	ret
endl:
	mov al, 0x0a					; line feed
	call putchar
	mov al, 0x0d					; carriage return
	call putchar
	ret

;	var al, *si;
;	lodsb(){
;		al = *si;
;		si++;
;	}

;    var al, *di;
;    stosb(){
;        *di = al;
;        di++;
;    }

prints:							; mov si, string
	.loop:
		lodsb					; bota character em al 
		cmp al, 0
		je .endloop
		call putchar
		jmp .loop
	.endloop:
	ret
reverse:						; mov si, string
	mov di, si
	xor cx, cx					; zerar contador
	.loop1:						; botar string na stack
		lodsb
		cmp al, 0
		je .endloop1
		inc cl
		push ax
		jmp .loop1
	.endloop1:
	.loop2: 					; remover string da stack				
		pop ax
		stosb
		loop .loop2
	ret
tostring:						; mov ax, int / mov di, string
	push di
	.loop1:
		cmp ax, 0
		je .endloop1
		xor dx, dx
		mov bx, 10
		div bx					; ax = 9999 -> ax = 999, dx = 9
		xchg ax, dx				; swap ax, dx
		add ax, 48				; 9 + '0' = '9'
		stosb
		xchg ax, dx
		jmp .loop1
	.endloop1:
	pop si
	cmp si, di
	jne .done
	mov al, 48
	stosb
	.done:
		mov al, 0
		stosb
		call reverse
		ret
gets:							; mov di, string
	xor cx, cx					; zerar contador
	.loop1:
		call getchar
		cmp al, 0x08			; backspace
		je .backspace
		cmp al, 0x0d			; carriage return
		je .done
		cmp cl, 10				; string limit checker
		je .loop1
		
		stosb
		inc cl
		call putchar
		
		jmp .loop1
		.backspace:
			cmp cl, 0			; is empty?
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

start:
	xor ax, ax
	mov ds, ax
	mov es, ax
	
	mov ax, 00h
	int 10h
	
    mov di, string
    call gets
    mov si, string
	call reverse
    mov si, string
    mov cx, 1
    xor ax,ax
	
    .loop5:
        push ax
        lodsb
        cmp al,0
        je .endloop5
        xor ah,ah
        sub al,48
        mul cx
        pop bx
        add ax,bx
        push ax
        mov ax,cx
        mov bx,10
        mul bx
        mov cx,ax
        pop ax
        jmp .loop5

    .endloop5:
        pop ax
        mov bx,ax
        mov cx,1
        xor ax,ax
    
	.loop6:
		cmp cx,bx
        ja .endloop6
        add ax,cx
        inc cx
        jmp .loop6
                
	.endloop6:
        mov di,string
        call tostring
        mov si, string
        call prints
                
    
	
times 510-($-$$) db 0
dw 0xaa55
