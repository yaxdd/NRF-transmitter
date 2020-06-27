    #include <p18f4550.inc>
    #include "fuses.inc"
    #include "SPI.inc"
    #include "NRF24L01.inc"
    #include "TIMER0.inc"
variables UDATA_ACS 0x00
W_TEMP	    res 1
STATUS_TEMP res 1
BSR_TEMP    res 1
TXDATA	  res 1
RXDATA	  res 1
TMR0_COUNT  res 2
DATA_TO_SEND     res 1
COMMAND	  res 1
BUFFER_DATA res 5

RES_VECT  CODE    0x0000	;declaramos el vector de reset
    GOTO    START

ISRHV     CODE    0x0008
    GOTO    HIGH_ISR
ISRLV     CODE    0x0018
    GOTO    LOW_ISR

ISRH      CODE
HIGH_ISR
			      ;guardamos el contexto al entrar  a la interrupcion
    movwf W_TEMP
    movff STATUS, STATUS_TEMP
    movff BSR, BSR_TEMP
    btfss INTCON,TMR0IF	      ;reviso la bandera de interrupcion del timer0
    bra EXIT_ISR		      ; si no se cumplió, salgo de la interrupcion
    BCF INTCON,TMR0IF	      ; Limpiamos la bandera
    bcf T0CON,TMR0ON	      ;desactivo el timer
    EXIT_ISR
			      ;Recuperamos el contexto
    movf W_TEMP,W
    movff STATUS_TEMP,STATUS
    movff BSR_TEMP,BSR
    retfie
ISRL      CODE
LOW_ISR
    retfie


MAIN_PROG CODE			;generamos la seccion de codigo de nuestro programa
; 1.- Configurar un puerto como analogo
; 2.- Configurar ADC
; 3.- Configurar SPI
; 4.- Leer dato
; 5.- Enviar dato
START
    ;clrf TRISB
    ;seleccionamos el oscilador interno y lo configuramos a 8 mhz
    movlw 0x72
    movwf OSCCON
    ;Inicializacion de los puertos

    ;Configuramos el puerto AN0 como analogo, los demas como digitales
    movlw 0x0E	;Voltages de referencia, vss y vdd, seleccionamos AN0 como analogo
    movwf ADCON1
    clrf ADCON0	;seleccionamos el canal AN0 y mantenemos apagado el ADC
    movlw 0xB9
    movwf ADCON2
    bsf ADCON0,ADON
    ;configuro las interrupciones
    clrf PIR1	    ;limpiamos bandera de interrupciones
    clrf INTCON	    ;limpiamos el registro
    clrf PIE1	    ;desactivamos todas las interrupciones de perifericos
    bsf INTCON,GIE	    ;activamos las interrupciones globales
    bsf INTCON,TMR0IE  ; y la interrupcion del timer0

    call TMR0_INIT
    call SPI_INIT
    call NRF_INIT_TX
    read_adc
    bsf ADCON0,GO
    wait_adc
    btfss PIR1,ADIF
    bra wait_adc
    bcf PIR1,ADIF
    movff ADRESH,BUFFER_DATA+1
    movff ADRESL,BUFFER_DATA
    call NRF_SEND_DATA
    bra read_adc		  ;esperamos las interrupciones
    END

