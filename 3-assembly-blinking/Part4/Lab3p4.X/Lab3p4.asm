#include <P18F4620.inc>
 config OSC = INTIO67
 config WDT = OFF
 config LVP = OFF
 config BOREN = OFF

 
 ;Constant declarations 
 Delay1 equ 0xFF;		; make delay1 255 seconds
 Delay2 equ 0XFF;
 
 Counter_L equ 0x20		; Counter_L equals value 32
 Counter_H equ 0x21;		; Counter_H equals value 33
 
 Color_Value equ 0x28;		; Color_Value equals 40
 Loop_Count equ 0x29;		; Loop_count equals 41

ORG 0x0000 

; CODE STARTS FROM THE NEXT LINE
 
START:
 ORG 0x0000
 
START:

 MOVLW 0x0F		; Load WREG with 0x0F
 MOVWF ADCON1		; Set ADCON1 to 0000 1111 so all digital
 MOVLW 0x00		; Load WREG with 0000 0000
 MOVWF TRISC		; Make PORTB bits 0-7 as outputs

 
WHILE_LOOP:		
 MOVLW 0x00		; 
 MOVWF Color_Value	; Set Color_Value to 00
 MOVLW 0x08		; 
 MOVWF Loop_Count	; Set Loop_Count to 8
 
 FOR_LOOP:
 MOVF Color_Value,W	; 
 MOVWF PORTC		; Set PORTC to Color_Value
 CALL DELAY_ONE_SEC	; Delay 1 sec
 INCF Color_Value,F	; Increment Color_Value
 DECF Loop_Count,F	; Decrement Loop_Count
 BNZ FOR_LOOP		; Branch back to FOR_LOOP if zero flag is not set
 
 GOTO WHILE_LOOP	; Jump to WHILE_LOOP

 
 
DELAY_ONE_SEC:
MOVLW Delay1		;
MOVWF Counter_H		;
LOOP_OUTER:
NOP ;
MOVLW Delay2 ;
MOVWF Counter_L ;
LOOP_INNER:
NOP ;
DECF Counter_L,F ;
BNZ LOOP_INNER ;
DECF Counter_H,F ;
BNZ LOOP_OUTER ;
RETURN 
    
 END 
