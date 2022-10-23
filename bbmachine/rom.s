; -*- asm -*-
;
; ROM code and interrupt Vectors for "bbmachine"
;
; Brent Burton
; Dec 2021
;
.include "bbmac.inc"
.include "zeropage.inc"

.import start

.export println, print, printchar, printhexbyte, ConsClear, ConsFlush
.export nmi_vec, res_vec, irq_vec

	.segment "ROM"
	
;;; PRINTLN subroutine - prints string buffer with newline
;;; Param: ptr1(zp) contains address of null-terminated string.
;;; Uses A,Y.
.proc println
	ldy #0				    ; char index
next:	lda (ptr1),y			    ; next char in A
	beq flush			    ; if Z, end of string.
	
	sta CONS_DATA1			    ; write char to data reg
	
	lda #CONS_CMD_PRINT		    ; write print cmd to ctrl reg
	sta CONS_CTRL
	
	iny				    ; incr index
	jmp next			    ; loop
	
flush:	jsr ConsFlush
	rts
.endproc

;;; PRINT subroutine - prints string buffer without flushing (no newline)
;;; Caller can call PRINT several times, then ConsFlush to flush/newline.
;;; Param: ptr1(zp) contains address of null-terminated string.
;;; Uses A,Y.
.proc print
	ldy #0				    ; char index
next:	lda (ptr1),y			    ; next char in A
	beq done			    ; if Z, end of string.
	
	sta CONS_DATA1			    ; write char to data reg
	
	lda #CONS_CMD_PRINT		    ; write print cmd to ctrl reg
	sta CONS_CTRL
	
	iny				    ; incr index
	jmp next			    ; loop
	
done:	rts
.endproc

;; PRINTCHAR - prints a single character given in A
.proc printchar
	sta CONS_DATA1
	lda #CONS_CMD_PRINT		    ; write print cmd to ctrl reg
	sta CONS_CTRL
	rts
.endproc

;;; ---------------- Print hex byte
;;; Value to print is in A.
hexstring: .byte "0123456789ABCDEF"	    ; converts indices 0-15 to character

.proc printhexbyte
	phx				    ; save X
	pha				    ; save a copy
	and #$F0			    ; get high nybble
	clc				    ; shift right 4
	ror
	ror
	ror
	ror
	tax				    ; index into hex string via X
	lda hexstring,X
	jsr printchar
	
	pla 				    ; get copy back
	and #$0F			    ; get low nybble
	tax
	lda hexstring,X
	jsr printchar
	plx				    ; restore X
	rts
.endproc

;;; ConsClear sends the clear command to the console.
;;; Uses A.
.proc ConsClear
	lda #CONS_CMD_CLEAR		    ; write clear command
	sta CONS_CTRL
	rts
.endproc

;;; ConsFlush sends the flush command to the console.
;;; Uses A
.proc ConsFlush
	lda #CONS_CMD_FLUSH		    ; write clear command
	sta CONS_CTRL
	rts
.endproc

;;;
;;; NMI trap
.proc nmi_vec
	rti
.endproc

;;;
;;; Reset
.proc res_vec
	jmp start
.endproc

;;;
;;; IRQ trap
.proc irq_vec
	rti
.endproc

;;; -------------- vectors ---------
	.segment "VECTORS"
        .word   nmi_vec
        .word   res_vec
        .word   irq_vec
