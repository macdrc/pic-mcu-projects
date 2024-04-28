
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
                            // Create an array with the assigned colors
char array[8] = {0xA6, 0x22, 0xE7, 0x03, 0x84, 0x65, 0x40, 0xC1};

void main()
{
char in;                    // Use variable 'in' as char
 TRISA = 0xFF;              // Set PORTA pins as inputs
 TRISB = 0x00;              // Set PORTB pins as outputs
 TRISC = 0x00;              // Do the same for PORTC and PORTD
 TRISD = 0x00;
 ADCON1 = 0x0f;             // Set the first 4 bits of PORTA to digital inputs

 while (1)                  // Endless loop
{
    for (int i=0; i<8; i++)
    {
        PORTC = i;          // Assign the data (bx0000 to bx1000) to PORTC
        PORTD = array[i];   // Assign the array to PORTD
        Delay_One_Sec();    // Call delay function
    }
}
} 