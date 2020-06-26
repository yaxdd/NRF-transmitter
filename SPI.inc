SPI_ROUTINE CODE ;DEJAMOS QUE EL LINKER SE ENCARGUE DE COLOCAR EL CODIGO

SPI_INIT
    bsf TRISB, RB0 ;configurado como digital en ADCON1
    bcf TRISC, RC7
    bcf TRISB, RB1
    bcf TRISA, RA5 ;configurado como digital en ADCON1
    movlw 0xC0	    ; [SMP:1][CKE:0][i2cBits:000000]
    movwf SSPSTAT
    movlw 0x20	    ; [WCOL:0][SSPOV:0][SSPEN:1][CKP:0][SSPM3-SSPM0:0000]
    movwf SSPCON1
    return
SPI_CLOSE
    movlw 0xDF
    andwf SSPCON1,1   ; aplicamos un and a SSPCON1, guardamos en SSPCON1
    return
SPI_READ
    movlw 0x00
    movwf SSPBUF
    esperar_lectura
    btfss SSPSTAT,BF
    bra esperar_lectura
    movf SSPBUF,W
    movwf RXDATA
    return
SPI_WRITE
    bcf PIR1,SSPIF
    movf SSPBUF,W	 ;Leer siempre
    movff TXDATA,SSPBUF  ;cargamos el dato a enviar
 esperar
    btfss SSPSTAT,BF
    bra esperar
    return




