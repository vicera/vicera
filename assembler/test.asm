; VICERA ASSEMBLER TESTING FILE
;
; This file contains all possible opcodes that can be
; put in the assembler. While debugging the assembler,
; it should show all those instruction in the numerical
; order.
;
halt	; Also a testing comment
nop

; Test comments
push HL	; Another comment
push BC
push DE
pusha

pop HL
pop BC
pop DE
popa

mov A, A
mov A, B
mov A, C
mov A, D
mov A, E
mov A, H
mov A, L

mov B, A
mov B, B
mov B, C
mov B, D
mov B, E
mov B, H
mov B, L

mov C, A
mov C, B
mov C, C
mov C, D
mov C, E
mov C, H
mov C, L

mov D, A
mov D, B
mov D, C
mov D, D
mov D, E
mov D, H
mov D, L

mov E, A
mov E, B
mov E, C
mov E, D
mov E, E
mov E, H
mov E, L

mov H, A
mov H, B
mov H, C
mov H, D
mov H, E
mov H, H
mov H, L

mov L, A
mov L, B
mov L, C
mov L, D
mov L, E
mov L, H
mov L, L

mov (HL), A
mov (HL), B
mov (HL), C
mov (HL), D
mov (HL), E
mov (HL), H
mov (HL), L

mov A, 0xff
mov B, 255
mov C, 0377
mov D, ' '
mov E, &ff
mov H, 0b11111111
mov L, 255

mov (HL), 0xff

mov A, (HL)
mov B, (HL)
mov C, (HL)
mov D, (HL)
mov E, (HL)
mov H, (HL)
mov L, (HL)

mov SP, myLabel

mov A, (BC)
mov A, (DE)
mov A, (0xffff)

mov (BC), A
mov (DE), A
mov (0xffff), A

mov HL, 0xffff
mov BC, 0xffff
mov DE, 0xffff

add A
add B
add C
add D
add E
add H
add L

add 0xff
add (HL)
