TIMER0_ROUTINES CODE

TMR0_INIT
			;configuro el timer 0
    CLRF T0CON		;limpiamos el registro
    bcf T0CON,T08BIT	;modo 16 bits
    bcf T0CON,T0CS	; seleccionmos el osc interno como reloj
    bcf T0CON,PSA	;asignamos el preescaler
    bsf T0CON,T0PS2	; a 32
    bcf T0CON,T0PS1
    bcf T0CON,T0PS0
    return
delay
    btfsc T0CON,TMR0ON
    bra delay
    return
TMR0_RELOAD
    bcf T0CON,TMR0ON		; desactivo timer0
    movff TMR0_COUNT,TMR0L
    movff TMR0_COUNT+1,TMR0H
    bsf T0CON,TMR0ON		; iniciar timer0
    return
DELAY_100MS
    movlw 0xE7
    movwf TMR0_COUNT+1
    movlw 0x95
    movwf TMR0_COUNT
    call TMR0_RELOAD
    goto delay
DELAY_10MS
    movlw 0xFD
    movwf TMR0_COUNT+1
    movlw 0x8E
    movwf TMR0_COUNT
    call TMR0_RELOAD
    goto delay
DELAY_1MS
    movlw 0xFF
    movwf TMR0_COUNT+1
    movlw 0xC0
    movwf TMR0_COUNT
    call TMR0_RELOAD
    goto delay
