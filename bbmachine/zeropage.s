;                                                                 -*- asm -*-
; Default zeropage for the bbmachine
;
; Brent Burton
; Dec 2021
;

.include "bbmac.inc"

.exportzp ptr1, mulres

;;; ----------------
	.ZEROPAGE
	.org $0008			    ; reserve first 8 bytes

ptr1:	.word $0000			    ; string pointer param for print()
mulres: .word $0000			    ; 16b result of mult8



