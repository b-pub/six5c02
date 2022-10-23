;                                                                 -*- asm -*-
; Default zeropage for the bbmachine
;
; Brent Burton
; Dec 2021
;

.include "bbmac.inc"

.exportzp ptr1, mulres, NUM1, NUM2

;;; ----------------
	.ZEROPAGE
	.org 8				  ; reserve first 8 bytes

ptr1:	.word	0	       ; string pointer param for print()
mulres: .word	0	       ; 16b result of mult8x8
NUM1:	.byte	0
NUM2:	.byte	0
