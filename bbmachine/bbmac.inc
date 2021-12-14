;                                                                 -*- asm -*-
; Definitions for "bbmachine"
;
; Brent Burton
; Dec 2021
;

;---------------- Program Info ----------------

DATA  = $0200
ENTRY = $0400

.PC02

;------------ Console Device -----------
; Control and data registers.
; Write data register first, then control register to process.
;
CONS_CTRL  = $BF00
CONS_DATA1 = $BF01

; Console commands
CONS_CMD_PRINT = $00			    ; set char into DATA1, then PRINT into CTRL.
CONS_CMD_FLUSH = $01			    ; set FLUSH to CTRL; no data.
CONS_CMD_CLEAR = $02			    ; set CLEAR to CTRL; no data.
;--------------------------------

