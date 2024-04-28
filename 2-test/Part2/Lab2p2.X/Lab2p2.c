#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF


void main()
{
char in;                    // Use variable 'in' as char
 TRISA = 0xFF;              // Set PORTA pins as inputs
 TRISB = 0x00;              // Set PORTB pins as outputs
 TRISC = 0x00;              // Do the same for PORTC and PORTD
 TRISD = 0x00;
 ADCON1 = 0x0f;             // Set the first 4 bits of PORTA to digital inputs

 while (1)                  // endless loop
{
    in = PORTA;             // read data from PORTA and save it
                            
    in = in & 0x0F;         // Mask out the unused upper four bits & (0000 1111)
                            // while preserving the lower 4-bits
    PORTC = in;             // Output the data to PORTC
    
}
 } 