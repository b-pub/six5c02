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

.export start
.proc start
	PUTLN hello
	PUTLN goodbye
	stp
.endproc
