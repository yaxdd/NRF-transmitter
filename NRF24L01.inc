;Definicion de constantes
R_REGISTER EQU 0x00
W_REGISTER EQU 0x20
W_TX_PAYLOAD EQU 0xA0
FLUSH_TX EQU 0xE1
FLUSH_RX EQU 0xE2

CONFIG_VAL EQU 0x00
EN_AA   EQU 0x01
EN_RXADDR   EQU 0x02
SETUP_AW    EQU 0x03
SETUP_RETR  EQU 0x04
RF_CH	    EQU 0x05
RF_SETUP    EQU 0x06
STATUS_VAL	    EQU 0x07
OBSERVE_TX  EQU 0x08
CD	    EQU 0x09
RX_ADDR_P0  EQU 0x0A
RX_ADDR_P1  EQU 0x0B
RX_ADDR_P2  EQU 0x0C
RX_ADDR_P3  EQU 0x0D
RX_ADDR_P4  EQU 0x0E
RX_ADDR_P5  EQU 0x0F
TX_ADDR	    EQU 0x10
RX_PW_P0    EQU 0x11
RX_PW_P1    EQU 0x12
RX_PW_P2    EQU 0x13
RX_PW_P3    EQU 0x14
RX_PW_P4    EQU 0x15
RX_PW_P5    EQU 0x16
FIFO_STATUS EQU 0x17
NRF_ROUTINE CODE ;DEJAMOS QUE EL LINKER SE ENCARGUE DE COLOCAR EL CODIGO

NRF_WRITE_REGISTER
    bcf PORTB, RB2
    movf COMMAND,W
    iorlw W_REGISTER
    movwf TXDATA
    call SPI_WRITE
    movf DATA_TO_SEND,W
    movwf TXDATA
    call SPI_WRITE
    bsf PORTB, RB2
    return
NRF_FLUSH
    bcf PORTB, RB2
    movlw FLUSH_TX
    movwf TXDATA
    CALL SPI_WRITE
    bsf PORTB, RB2
    return
NRF_WRITE_BUFFER_5
    bcf PORTB, RB2
    movf COMMAND,W
    iorlw W_REGISTER
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA,W
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA+1,W
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA+2,W
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA+3,W
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA+4,W
    movwf TXDATA
    call SPI_WRITE
    bsf PORTB, RB2
    return
NRF_WRITE_BUFFER_2
    bcf PORTB, RB2
    movf COMMAND,W
    iorlw W_REGISTER
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA+1,W
    movwf TXDATA
    call SPI_WRITE
    movf BUFFER_DATA,W
    movwf TXDATA
    call SPI_WRITE
    bsf PORTB, RB2
    return
NRF_SEND_DATA
    call NRF_FLUSH		;limpiamos las colas
    movf W_TX_PAYLOAD,W
    movwf COMMAND
    call NRF_WRITE_BUFFER_2
    movlw 0x70
    movwf DATA_TO_SEND
    movf STATUS_VAL,W
    movwf COMMAND
    call NRF_WRITE_REGISTER ;limpiamos las banderas
    movlw 0x02
    movwf DATA_TO_SEND
    movf CONFIG_VAL,W
    movwf COMMAND
    call NRF_WRITE_REGISTER ;colocamos en modo TX [prim_RX:0]
    bsf PORTB,RB3
    call DELAY_1MS	    ;espera de al menos 10us
    bcf PORTB,RB3
    return
NRF_SET_TX_MODE
    call NRF_FLUSH	    ;limpiamos las colas
    movlw 0x70
    movwf DATA_TO_SEND
    movf STATUS_VAL,W
    movwf COMMAND
    call NRF_WRITE_REGISTER ;limpiamos las banderas
    movlw 0x02
    movwf DATA_TO_SEND
    movf CONFIG_VAL,W
    movwf COMMAND
    call NRF_WRITE_REGISTER ;colocamos en modo TX [prim_RX:0]
    return
NRF_READ_REGISTER
    bcf PORTB, RB2
    movf COMMAND,W
    iorlw W_REGISTER
    movwf TXDATA
    call SPI_WRITE
    call SPI_READ
    bsf PORTB, RB2
    return
NRF_INIT_TX
    bcf TRISB,RB2	    ;CSN : CHIP SELECT NOT
    bcf TRISB,RB3	    ;CE	 : CHIP ENABLE
    bsf PORTB,RB2	    ; mantenemos en alto CSN
    bcf PORTB,RB3
			    ;hacemos un delay de 100 ms para iniciar el modulo
    call DELAY_100MS
			     ; para cada comando, debemos hacer una transicion de CSN en alto hacia abajo
			    ;cmd <msb-lsb>
			     ;data <lsb-msb>
    movlw 0x02		    ;[EN_CRC:0][CRC:0][PWR_UP:1][PRIM_RX:0]
    movwf DATA_TO_SEND	    ;valor que ira en el registro
    movlw CONFIG_VAL
    movwf COMMAND	    ;registro a editar
    call NRF_WRITE_REGISTER
			    ;hacemos un delay de 10 ms para iniciar el modulo
    call DELAY_10MS
			     ;tiempo terminado
    movlw 0x00		    ;[ENAA_P0:1]
    movwf DATA_TO_SEND
    movlw EN_AA
    movwf COMMAND
    call NRF_WRITE_REGISTER
    movlw 0x00			;[ERX_P0:0]
    movwf DATA_TO_SEND
    movlw EN_RXADDR
    movwf COMMAND
    call NRF_WRITE_REGISTER
    movlw 0x03			;[AW1-AW0:11]
    movwf DATA_TO_SEND
    movlw SETUP_AW
    movwf COMMAND
    call NRF_WRITE_REGISTER
    movlw 0x00			;[AW1-AW0:11]
    movwf DATA_TO_SEND
    movlw SETUP_RETR
    movwf COMMAND
    call NRF_WRITE_REGISTER
    movlw 0x40    ;CANAL
    movwf DATA_TO_SEND
    movlw RF_CH
    movwf COMMAND
    call NRF_WRITE_REGISTER
    movlw 0x26    ;[AW1-AW0:11]
    movwf DATA_TO_SEND
    movlw RF_SETUP
    movwf COMMAND
    call NRF_WRITE_REGISTER
    movlw RX_ADDR_P0
    movwf COMMAND
    movlw 0x05		;llenamos el buffer con la direccion de los del pipe 0
    movwf BUFFER_DATA
    movlw 0x04
    movwf BUFFER_DATA+1
    movlw 0x03
    movwf BUFFER_DATA+2
    movlw 0x02
    movwf BUFFER_DATA+3
    movlw 0x01
    movwf BUFFER_DATA+4
    call NRF_WRITE_BUFFER_5
    movlw TX_ADDR		  ;llenamos el buffer con la direccion del transmisor, igual que el pipe 0
    movwf COMMAND
    call NRF_WRITE_BUFFER_5
    movlw 0x00			   ;numero de bytes a enviar
    movwf DATA_TO_SEND
    movlw RX_PW_P0
    movwf COMMAND
    call NRF_WRITE_REGISTER
    return


