;                                                                 -*- asm -*-
; Hello World for the bbmachine
;

.include "bbmac.inc"

;;; ----------------
	.ZEROPAGE
ptr1:	.word $0100
cons:	.word $0302

;;; ----------------
	.DATA
hwstr:	.asciiz "Hello, World!"
	
;;; ----------------
	.assert * < $0400, error, "Data is too long, extends into $0400"
	.CODE
start:
	lda #<hwstr			    ; copy hwstr address to ptr1
	sta ptr1
	lda #>hwstr
	sta ptr1+1
	jsr print
	rts

;; ----------------------------------------------------------------
	.segment "ROM"

;;; PRINT subroutine
;;; Param: ptr1(zp) contains address of null-terminated string.
;;; Uses indirect indexed off Y via ptr1
print:
	ldy #0				    ; char index
next:	lda (ptr1),y
	beq flush			    ; if Z, end of string.
	phy				    ; save Y
	
	lda #<CONS_DATA1		    ; put data1 address into 'cons'
	sta cons
	lda #>CONS_DATA1
	sta cons+1
	lda #CONS_CMD_CLEAR		    ; write clear command
	ldy #0
	sta (cons),y
	
	ply				    ; restore index value
	iny
	jmp next
	
flush:	jsr ConsFlush
	rts

;;; ConsClear sends the clear command to the console.
;;; Uses cons(zp) to store I/O address, A, Y.
ConsClear:
	lda #<CONS_CTRL		    ; put control address into 'cons'
	sta cons
	lda #>CONS_CTRL
	sta cons+1
	lda #CONS_CMD_CLEAR		    ; write clear command
	ldy #0
	sta (cons),y
	rts

;;; ConsFlush sends the flush command to the console.
;;; Uses cons(zp) to store I/O address, A, Y.
ConsFlush:
	lda #<CONS_CTRL		    ; put control address into 'cons'
	sta cons
	lda #>CONS_CTRL
	sta cons+1
	lda #CONS_CMD_FLUSH		    ; write clear command
	ldy #0
	sta (cons),y
	rts

;;;
;;; NMI trap
nmi_vec:
	stp

;;;
;;; Reset
res_vec:
	jmp start

;;;
;;; IRQ trap
irq_vec:
	stp

	.segment "VECTORS"
        .word   nmi_vec
        .word   res_vec
        .word   irq_vec
