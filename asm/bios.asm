;
; VICERA by h34ting4ppliance
;
; asm/bios.asm
;
; This is a "BIOS" showing an animation kinda similar to the Gameboy BIOS boot
; screen.
;

; TODO Comment out everything.
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
	mov HL, 0x8247
	InsertTiles_L:
		inc A
		mov (HL), A
		inc HL

		cp 6
		jz InsertTiles_E
		jp InsertTiles_L
	InsertTiles_E:

	popa
	ret

MainLoop:
	pusha
	
	mov A, (0xfff2)
	cp 0x00
	jz MainLoop_A
	jp MainLoop_B

	MainLoop_A:
		mov A, (0xfff0)
		cp 0x80

		jn MainLoop_E
		halt
	MainLoop_B:
		sub 2
		mov (0xfff2), A
		jp MainLoop_E

	MainLoop_E:
		popa
		ret

Main:
	mov SP, 0xffef

	call LoadTiles
	call InsertTiles
	dumpm 0x8738
	
	; Testing
	mov A, 0x80
	mov (0xfff2), A

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
