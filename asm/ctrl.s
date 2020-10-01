; 
; VICERA by h34ting4ppliance
;
; asm/ctrl.asm
;
; This is a demo file to test the controller.
;
jp Main

Tiles:
	.db 0b00000000
	.db 0b00111100
	.db 0b01100010
	.db 0b01100010
	.db 0b01100010
	.db 0b01100010
	.db 0b00111100
	.db 0b00000000

	.db 0b00000000
	.db 0b00011000
	.db 0b00111000
	.db 0b01111000
	.db 0b00011000
	.db 0b00011000
	.db 0b01111110
	.db 0b00000000
Tiles_E:

LoadTiles:
	pusha

	mov HL, 0x8708
	mov DE, Tiles
	mov BC, Tiles_E
	LoadTiles_L:
		mov A, (DE)
		mov (HL), A
		
		mov A, D
		cp B
		jn LoadTiles_P
		mov A, E
		cp C
		jn LoadTiles_P

		jp LoadTiles_E
	LoadTiles_P:
		inc HL
		inc DE
		jp LoadTiles_L
	LoadTiles_E:

	popa
	ret

; CheckKeypress
; in  D - keycode
; out A - pressed?
CheckKeypress:
	pusha

	mov A, (0xfff4)
	mov B, A

	and D
	cp 0x00
	jz CheckKeypress_N

		mov A, 0x01
		jp CheckKeypress_E
	CheckKeypress_N:
		mov A, 0x00
	CheckKeypress_E:

	popa
	ret

MainLoop:
	pusha
	mov HL, 0x8246

	mov D, 0x80
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x40
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x20
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x10
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x08
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x04
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x02
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	mov D, 0x01
	call CheckKeypress
	inc A
	mov (HL), A
	inc HL

	popa
	ret

Main:
	mov SP, 0xffef

	call LoadTiles
	
	Main_L:
		mov A, (0xfff0)
		mov B, A
		WaitScr_L:
			mov A, (0xfff0)
			cp B
			slp
			jz WaitScr_L

		call MainLoop
		jp Main_L
