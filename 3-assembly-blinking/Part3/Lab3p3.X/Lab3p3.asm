; THIS SECOND ASSEMBLY LANGUAGE PROGRAM WILL READ THE VALUES OF
; ALL THE BITS 0-3 OF PORT A AND OUTPUT THEM
; TO THE PINS 0 THROUGH 3 OF PORT B
    
 
 #include <P18F4620.inc>
 config OSC = INTIO67
 config WDT = OFF
 config LVP = OFF
 config BOREN = OFF
 ORG 0x0000
 
START:
 MOVLW 0x0F 	    ; Load WREG with 0x0F
 MOVWF ADCON1	    ; Set ADCON1 to 0000 1111 so all digital
 MOVLW 0xFF	    ; Load WRED with 1111 1111
 MOVWF TRISA	    ; Set PORT A as all inputs
 MOVLW 0x00	    ; Load WREG with 0000 0000
 MOVWF TRISC	    ; Make PORTB bits 0-7 as outputs
 
MAIN_LOOP:	    ; Name the function
 MOVF PORTA, W	    ; Move PORTA value into WREG
 
 ANDLW 0x0F	    ; AND the literl value 0000 1111 with the WREG value
		    ; i.e. (0000 1111 & 0101 1001 = 0000 1001) elimiates first 4 bits
 
MOVWF PORTC	    ; Move WREG value into PORTB
 
 GOTO MAIN_LOOP	    ; Restart loop
 END