;                                                                 -*- asm -*-
; Symbols exported from ROM
;
; Brent Burton
; Dec 2021
;
.import println, print, printchar, printhexbyte, ConsClear, ConsFlush

;;; The PUTLN macro makes the println() function easier.
;;; A newline is printed.
;;;
.macro PUTLN  str
	lda #<str
	sta ptr1
	lda #>str
	sta ptr1+1
	jsr println
.endmacro

;;; The PUTS macro makes the print() function easier.
;;; No newline is printed.
;;;
.macro PUTS  str
	lda #<str
	sta ptr1
	lda #>str
	sta ptr1+1
	jsr print
.endmacro
	
