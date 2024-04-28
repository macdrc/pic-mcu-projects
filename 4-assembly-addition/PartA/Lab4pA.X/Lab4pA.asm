; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH LEDS
; CONNECTED TO THE PINS 0 THROUGH 3 OF PORT B
#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF

InA equ 0x20
InB equ 0x21
Result equ 0x22

ORG 0x0000
 ; CODE STARTS FROM THE NEXT LINE
 
START:
 MOVLW 0x0F		; Move into the working register 0x0F value
 MOVWF ADCON1		; Move whatever is in the WR into ADCON1
 
 MOVLW 0xFF		; Move 1111 1111 into WR
 MOVWF TRISA		
 MOVWF TRISB
 
 MOVLW 0x00
 MOVWF TRISC
 MOVWF TRISE
 
 MOVLW 0x07
 MOVWF TRISD
 
 
  
MAIN_LOOP:		; Start a loop
MOVF PORTA, W		; Move PORTA value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111

MOVWF InA		; store in InA
    
COMF InA,W		; Compliment LnA with WREG

MOVWF Result		; Output to Result
MOVFF Result, PORTC	; Move Result to PORTC
    
    BZ Set_ZFlag1
    GOTO Clear_ZFlag1
    
    Clear_ZFlag1:
    BCF PORTE, 2
    GOTO DONE5
    
    Set_ZFlag1:
    BSF PORTE, 2
    
    DONE1:
    
 GOTO MAIN_LOOP		; Return to beginning of MAIN_LOOP
    
RETURN
    
 END 

