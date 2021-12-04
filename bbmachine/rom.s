; -*- asm -*-
;
; ROM and vectors for "bbmachine"
;
.PC02

	.segment "ROM"
.export print, nmi_vec, res_vec, irq_vec

;;; PRINT subroutine
;;; Accepts address of ASCII zero-terminated string to print.
;;;  A: lo(addr)  X: hi(addr)
;;; Print uses ZP scratch location 'prword'
print:
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
