; Vicera boot BIOS
; Not really a BIOS but just a cool animation.
jp main

Tileset:
	db 0b00000000
	db 0b11000110
	db 0b11001100
	db 0b11011000
	db 0b11110000
	db 0b11100000
	db 0b11000000
	db 0b00000000

	db 0b00000000
	db 0b01111110
	db 0b00011000
	db 0b00011000
	db 0b00011000
	db 0b00011000
	db 0b01111110
	db 0b00000000

	db 0b00000000
	db 0b00111100
	db 0b01100110
	db 0b01100000
	db 0b01100000
	db 0b01100110
	db 0b00111100
	db 0b00000000

	db 0b00000000
	db 0b01111110
	db 0b01100000
	db 0b01111000
	db 0b01100000
	db 0b01100000
	db 0b01111110
	db 0b00000000

	db 0b00000000
	db 0b01111100
	db 0b01100010
	db 0b01111100
	db 0b01101000
	db 0b01100100
	db 0b01100010
	db 0b00000000

	db 0b00000000
	db 0b00111100
	db 0b01100010
	db 0b01100010
	db 0b01111110
	db 0b01100010
	db 0b01100010
	db 0b00000000
Tileset_E:

; Loads all the tiles
LoadTiles:
	pusha

	mov HL, 0x8708
	mov DE, Tileset
	mov BC, Tileset_E
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

InsertTiles:
	pusha

	mov A, 0
	mov H, 0x81
	InsertTiles_L:
		inc A
		mov L, A
		mov (HL), A

		cp 6
		jz InsertTiles_E
		jp InsertTiles_L
	InsertTiles_E:

	popa
	ret

MainLoop:
	pusha
	
	mov A, (0x9000)
	sub 1
	mov (0xfff1), A
	mov (0xfff2), A
	mov (0x9000), A

	popa
	ret

Main:
	mov SP, 0xffef

	call LoadTiles
	call InsertTiles
	dumpm 0x8738
	
	; Testing

	EndlessLoop:
		mov A, (0xfff0)
		mov B, A
		WaitScr_L:
			mov A, (0xfff0)
			cp B
			slp
			jz WaitScr_L

		call MainLoop
		jp EndlessLoop
