
#include<P18F4620.inc>
config OSC = INTIO67
config WDT = OFF
config LVP = OFF
config BOREN = OFF

InA equ 0x01
InB equ 0x02
Result equ 0x03

ORG 0x0000
 ; CODE STARTS FROM THE NEXT LINE
 
 
START:
 MOVLW 0x0F		; Move into the working register 0x0F value
 MOVWF ADCON1		; Move whatever is in the WR into ADCON1
 
 MOVLW 0xFF		
 MOVWF TRISA
 MOVLW 0xFF
 MOVWF TRISB
 
 MOVLW 0x00
 MOVWF TRISC
 MOVLW 0x00
 MOVWF TRISE
 
 MOVLW 0x07
 MOVWF TRISD 
 
 
 
MAIN_LOOP:
    BTFSC PORTD, 2	    ; if bit 2 clear then skip line, if not go down 1 line
    GOTO PORTD2EQ1
    GOTO PORTD2EQ0

PORTD2EQ1:
    GOTO TASK_BCD

PORTD2EQ0:
    BTFSC PORTD, 1	    ; if bit 1 clear, skip
    GOTO PORTD21EQ01   
    GOTO PORTD21EQ00

PORTD21EQ01:
 BTFSC PORTD, 0	    ; if bit 0 clear, skip
 GOTO TASK_OR
 GOTO TASK_XOR
    
PORTD21EQ00:
 BTFSC PORTD, 0     ; if bit 0 clear, skip 
 GOTO TASK_ADD
 GOTO TASK_COMP
 
 
TASK_COMP:
 BCF PORTD, 5
 BCF PORTD, 4
 BCF PORTD, 3
 CALL SUBROUTINE_COMP
 GOTO MAIN_LOOP
 
TASK_ADD:
 BCF PORTD, 5
 BCF PORTD, 4
 BSF PORTD, 3
 CALL SUBROUTINE_ADD
 GOTO MAIN_LOOP  
 
 TASK_XOR:
 BCF PORTD, 5
 BSF PORTD, 4
 BCF PORTD, 3
 CALL SUBROUTINE_XOR
 GOTO MAIN_LOOP
 
 TASK_OR:
 BCF PORTD, 5
 BSF PORTD, 4
 BSF PORTD, 3
 CALL SUBROUTINE_OR
 GOTO MAIN_LOOP
 
 TASK_BCD:
 BSF PORTD, 5
 BCF PORTD, 4
 BCF PORTD, 3
 CALL SUBROUTINE_BCD
 GOTO MAIN_LOOP
 
 
 
 
 
   

    
SUBROUTINE_COMP:	
MOVF PORTA, W		; Move PORTA value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111

MOVWF InA		; store in InA
    
COMF InA, W		; Compliment LnA with WREG
ANDLW 0x0F
MOVWF Result		; Output to Result
MOVFF Result, PORTC	; Move Result to PORTC
    
    
BZ Set_ZFlag1
GOTO Clear_ZFlag1
    
Clear_ZFlag1:
BCF PORTE, 2
GOTO DONE1
    
Set_ZFlag1:
BSF PORTE, 2
    
DONE1:
    
RETURN
    

    
    
    
SUBROUTINE_ADD:		
MOVF PORTA, W		; Move PORTA value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111
MOVWF InA		; store in InA 
    
MOVF PORTB, W		; Move PORTB value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111
MOVWF InB		; store in InB
    
MOVWF Result		; Put WREG into Result
ADDWF InA, W		; ADD LnA to WREG
MOVWF Result		; Put WREG into Result
    
MOVFF Result, PORTC	; Move Result to PORTC

    
BZ Set_ZFlag2
GOTO Clear_ZFlag2
    
Clear_ZFlag2:
BCF PORTE, 2
GOTO DONE2
    
Set_ZFlag2:
BSF PORTE, 2
    
DONE2:
    
RETURN
    
    

    
    
SUBROUTINE_XOR:		
    
MOVF PORTA, W		; Move PORTA value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111
MOVWF InA		; store in InA 
    
MOVF PORTB, W		; Move PORTB value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111
MOVWF InB		; store WREG (InB) in InB
    
MOVWF Result		; Put WREG (InB) into Result
XORWF InA, W		; XOR LnA with WREG (InB)
MOVWF Result		; Put WREG into Result
    
MOVFF Result, PORTC	; Move Result to PORTC
    
BZ Set_ZFlag3
GOTO Clear_ZFlag3
    
Clear_ZFlag3:
BCF PORTE, 2
GOTO DONE3
    
Set_ZFlag3:
BSF PORTE, 2
    
DONE3:
    
RETURN
    
    
    
    
    
    
SUBROUTINE_OR:		
    
MOVF PORTA, W		; Move PORTA value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111
MOVWF InA		; store in InA 
    
MOVF PORTB, W		; Move PORTB value into WREG
ANDLW 0x0F		; AND the WREG value with 0000 1111
MOVWF InB		; store WREG (InB) in InB
    
MOVWF Result		; Put WREG (InB) into Result
IORWF InA, W		; OR InA with WREG (InB)
MOVWF Result		; Put WREG into Result
    
MOVFF Result, PORTC	; Move Result to PORTC
    
BZ Set_ZFlag4
GOTO Clear_ZFlag4
    
Clear_ZFlag4:
BCF PORTE, 2
GOTO DONE4
    
Set_ZFlag4:
BSF PORTE, 2
    
DONE4:
    
RETURN
    
    
    
SUBROUTINE_BCD:
  
MOVF PORTA, W		; Move PORTA into WREG
    
ANDLW 0x0F		; Mask out bits
MOVWF InA		; Move WREG into InA

    
MOVLW 0x09		; move constant 9 into WREG
   
CPFSGT InA,1		;;If input_a is Greater than 0x09 move on to next line
    GOTO OUTPUT
    MOVLW 0x06
    ADDWF InA,0			; add 6 to WREG
    MOVWF Result		; add WREG to Result    
    

    BZ Set_ZFlag5
    GOTO Clear_ZFlag5
    
    Clear_ZFlag5:
    BCF PORTE, 2
    GOTO DONE5
    
    Set_ZFlag5:
    BSF PORTE, 2
    
    OUTPUT:
    MOVF    InA,W	    ;Read from input_a and move into W
    MOVWF   Result	    ;Move W into result
    MOVFF   Result, PORTC   ;Move content from result to PORTC
    
    DONE5:
    RETURN
    
    
   
    
    END
   