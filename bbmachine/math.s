;                                                                 -*- asm -*-
; Math functions for the bbmachine
;
; Brent Burton
; Dec 2021
;

.include "bbmac.inc"
.include "zeropage.inc"
.include "rom.inc"

;;; ----------------
	.DATA
startmsg:   .asciiz "Starting mults..."
donemsg:    .asciiz "Done."
productmsg: .asciiz "128x8 = 0x"

;;; ----------------
	.CODE

.export start
.proc start
	jsr ConsFlush
	PUTLN startmsg

	;; Multiply 128 by 8
	lda #128
	sta NUM1
	lda #8
	sta NUM2
	jsr mult8x8

	;; Print result
	PUTS productmsg
	lda mulres+1
	jsr printhexbyte
	lda mulres
	jsr printhexbyte
	jsr ConsFlush

	PUTLN donemsg
	jsr ConsFlush
	stp
.endproc

;;; -------- Unsigned 8b multiplication
;;; https://llx.com/Neil/a2/mult.html
;;;
.proc mult8x8
        lda #$80     ;Preload sentinel bit into RESULT
        sta mulres
        asl A        ;Initialize RESULT hi byte to 0
L1:	lsr NUM2     ;Get low bit of NUM2
        bcc L2       ;0 or 1?
        clc          ;If 1, add NUM1
        adc NUM1
L2:	ror A        ;"Stairstep" shift (catching carry from add)
        ror mulres
        bcc L1       ;When sentinel falls off into carry, we're done
        sta mulres+1
	rts
.endproc

;;; ---------------- Dump memory
;;; 
.proc dumpmem
	rts
.endproc
