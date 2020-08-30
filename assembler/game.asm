; Testing assembly file
jp BEGIN

my_sprite:
	db 0b11111111
	db 0b10000001
	db 0b10000001
	db 0b10000001
	db 0b10000001
	db 0b10000001
	db 0b10000001
	db 0b11111111

LOAD_SPRITES:
	; setting up addresses
	xor C

	mov BC, my_sprite
	mov HL, 0x8500

	; move 8 bytes to the sprite
	; memory in the VRAM
	LOAD_SPRITES_LOOP:
		mov A, (BC)
		inc BC

		mov (HL), A
		inc HL

		inc C
		mov A, 8
		cp C
		
		jz LOAD_SPRITES_END		; This is an useless comment
		jp LOAD_SPRITES_LOOP	; to test the comment parsing.
	
	; Set up a slot in the sprite
	; index
	LOAD_SPRITES_END:
		mov HL, 0x8000
		
		mov A, 0x00
		mov (HL), A
		inc HL

		mov A, 0x01
		mov (HL), A
	
	ret

; Beginning of the computer
; program
BEGIN:
	call LOAD_SPRITES
	
	mov HL, 0x8002
	mov DE, 0xfff0
	xor A
	jp MAINLOOP

MAINLOOP:
	; moving sprite
	mov (HL), A
	inc HL
	mov (HL), A
	dec HL

	; Waiting for screen update
	WAITSCREEN:
		cp (DE)
		jc MAINLOOP

		mov A, (DE)
		jp WAITSCREEN
