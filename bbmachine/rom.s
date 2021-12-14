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

.export print, ConsClear, ConsFlush
.export nmi_vec, res_vec, irq_vec

	.segment "ROM"
	
;;; PRINT subroutine
;;; Param: ptr1(zp) contains address of null-terminated string.
;;; Uses A,Y.
.proc print
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
