   
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
 D2_Start_Value equ 0x30	; 
 D3_Start_Value equ 0x38
 
; Constant declarations


ORG 0x0000 ;

; CODE STARTS FROM THE NEXT LINE
 
START:
 ORG 0x0000
 
 MOVLW 0x0F 
 MOVWF ADCON1 
 MOVLW 0x00 
 MOVWF TRISC 
 MOVWF TRISD
 MOVWF TRISE
 
 MOVLW 0x18
 MOVWF D2_Start_Value
 MOVLW 0x08
 MOVWF D2_Start_Value+1;
 MOVLW 0x19
 MOVWF D2_Start_Value+2
 MOVLW 0x09
 MOVWF D2_Start_Value+3;
 MOVLW 0x10
 MOVWF D2_Start_Value+4
 MOVLW 0x11
 MOVWF D2_Start_Value+5;
 MOVLW 0x00
 MOVWF D2_Start_Value+6;
 MOVLW 0x01
 MOVWF D2_Start_Value+7;
 
 MOVLW 0x05
 MOVWF D3_Start_Value
 MOVLW 0x01
 MOVWF D3_Start_Value+1
 MOVLW 0x07
 MOVWF D3_Start_Value+2
 MOVLW 0x00
 MOVWF D3_Start_Value+3
 MOVLW 0x04
 MOVWF D3_Start_Value+4
 MOVLW 0x03
 MOVWF D3_Start_Value+5
 MOVLW 0x02
 MOVWF D3_Start_Value+6
 MOVLW 0x06
 MOVWF D3_Start_Value+7
 
 MOVLW 0X34

WHILE_LOOP: ;
 LFSR 0,D2_Start_Value;
 LFSR 1,D3_Start_Value;
 MOVLW 0x00		    ;
 MOVWF Color_Value	    ; Set Color_Value to 0
 MOVLW 0x08 ;
 MOVWF Loop_Count ; 
 
FOR_LOOP:
 MOVF Color_Value,W	    ;
 MOVWF PORTC		    ; Set PORTC to Color_Value
 MOVF INDF0,W
 MOVWF PORTD
 MOVF INDF1,W
 MOVWF PORTE
 CALL DELAY_ONE_SEC ;
 INCF FSR0L
 INCF FSR1L
 INCF Color_Value,F ;
 DECF Loop_Count,F ;
 BNZ FOR_LOOP ;
 ;
 GOTO WHILE_LOOP ;

 
 
 DELAY_ONE_SEC:
MOVLW Delay1		    
MOVWF Counter_H		    
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
 
 