
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
    char IN;
 ADCON1 = 0x0f;
 TRISA = 0xff;
 TRISB = 0x00;
 while (1)
    {
    IN = PORTA & 0x0F;
    PORTB = IN;
    }
} 