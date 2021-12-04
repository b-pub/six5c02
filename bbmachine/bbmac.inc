; -*- asm -*-
; Definitions for "bbmachine"
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
CONS_CMD_PRINT = $00
CONS_CMD_FLUSH = $01
CONS_CMD_CLEAR = $02
;--------------------------------

