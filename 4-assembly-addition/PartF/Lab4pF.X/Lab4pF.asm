; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH LEDS
; CONNECTED TO THE PINS 0 THROUGH 3 OF PORT B
#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF

InA equ 0x20
InB equ 0x21
Result equ 0x21

ORG 0x0000
 ; CODE STARTS FROM THE NEXT LINE
 

SUBROUTINE_BCD:
  
MOVF PORTA, W		; Move PORTA into WREG
ANDLW 0x0F		; Mask out bits
MOVWF InA		; Move WREG into InA

    
MOVLW 0x09		; move constant 9 into WREG
   
CPFSGT InA,1		; compare WREG to InA
GOTO OUTPUT
    MOVLW 0x06
    ADDWF InA,0			; add 6 to WREG
    MOVWF Result		; add WREG to Result    
    
    BCF        PORTE,2         ; Clear bit 2 of PORTE 
    BNZ        CHECKBZ5        ; If no zero flag then jump to CHECKBZ5
    BSF        PORTE,2         ; Set bit 2 of PORT E
 
    CHECKBZ5:
RETURN        
