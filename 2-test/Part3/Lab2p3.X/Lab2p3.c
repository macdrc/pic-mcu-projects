#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

void Delay_One_Sec()        // Use a delay function 
{
    for (int I=0; I <17000; I++);
}

void main()
{
char in;                    // Use variable 'in' as char
 TRISA = 0xFF;              // Set PORTA pins as inputs
 TRISB = 0x00;              // Set PORTB pins as outputs
 TRISC = 0x00;              // Do the same for PORTC
 ADCON1 = 0x0F;             // Set the first 4 bits of PORTA to digital inputs

 while (1)                  // Endless loop
{
    for (int i=0; i<8; i++)
    {
    PORTC = i;              // Output the data (bx0000 to bx1000) to PORTC
    Delay_One_Sec();        // Call delay function
    }
}
} 
