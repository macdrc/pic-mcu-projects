; THIS FIRST ASSEMBLY LANGUAGE PROGRAM WILL FLASH LEDS
; CONNECTED TO THE PINS 0 THROUGH 3 OF PORT B
#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF
; Constant declarations
Delay1 equ 0xFF		; Delay1 = 255
Delay2 equ 0XFF		; Delay2 = 255
Counter_L equ 0x20	; Counter_L = 32
Counter_H equ 0x21	; Counter_H = 33
ORG 0x0000
 ; CODE STARTS FROM THE NEXT LINE
 
START:
 MOVLW 0x0F		; Move into the working register 0x0F value
 MOVWF ADCON1		; Move whatever is in the WR into ADCON1
 MOVLW 0x00		; Move 0 into WR
 MOVWF TRISB		; Set TRISB to 0 so all PORTB are outputs
 
MAIN_LOOP:		; Start a loop
MOVLW 0x05		; Move 5 into WR
MOVWF PORTB		; Set PORTB to 0101 (2nd and 4th LED will blink)

CALL DELAY_ONE_SEC	; Use the function DELAY_ONE_SEC
MOVLW 0x0A		; Move 10 into WR
MOVWF PORTB 		; Set PORTB to 1010 (1st and 3rd led will blink)
CALL DELAY_ONE_SEC	; Use the function DELAY_ONE_SEC
GOTO MAIN_LOOP		; Return to beginning of MAIN_LOOP

    
DELAY_ONE_SEC:  
MOVLW Delay1		; Move 255 into WR
MOVWF Counter_H		; Move 255 into Counter_H
    
LOOP_OUTER:
NOP			; Do nothing for 1 usec
MOVLW Delay2		; Move 255 into WR
MOVWF Counter_L		; Move 255 into Counter_L

LOOP_INNER:		    ; total time - 2us, total loop time - 0.5ms
NOP			; Do nothing for 1 cycle			    
DECF Counter_L,F	; Decrement Counter_L value (255 - 1)		    
BNZ LOOP_INNER		; Branch back to LOOP_INNER if zero flag is not set 
DECF Counter_H,F	; Decrement Counter_H, insert back into Counter_H
BNZ LOOP_OUTER		; Branch back to LOOP_OUTER if zero flag is not set

RETURN
    
 END 

