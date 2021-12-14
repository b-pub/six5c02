;                                                                 -*- asm -*-
; Hello World for the bbmachine
;
; Brent Burton
; Dec 2021
;

.include "bbmac.inc"
.include "zeropage.inc"
.include "rom.inc"

;;; ----------------
	.DATA
hello:	.asciiz "Hello, World!"
goodbye: .asciiz "Goodbye, World!"

;;; ----------------
	.CODE

.macro PUTS  str
	lda #<str		     ; copy hwstr address to ptr1
	sta ptr1
	lda #>str
	sta ptr1+1
	jsr print
.endmacro
	
.export start
.proc start
	PUTS hello
	PUTS goodbye
	stp
.endproc
