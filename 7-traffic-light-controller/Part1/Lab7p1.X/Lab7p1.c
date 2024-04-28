#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <usart.h>
#include <p18f4620.h> 

#pragma config OSC = INTIO67    
#pragma config WDT = OFF       
#pragma config LVP = OFF        
#pragma config BOREN = OFF      
#pragma warning disable 752
#define SEC_LED PORTDbits.RD7

void Init_ADC(); 
void init_UART(); 
void Init_TRIS();

void WAIT_1_SEC();
void WAIT_HALF_SEC();

void main(void)
{
    OSCCON = 0x60;                       // Set cycles to 1MHz     
    
    PORTB = 0x00; 
    PORTC = 0xFF;                   
    PORTD = 0xFF;                   
    PORTE = 0xFF;
   
    Init_ADC();                          // Initialize ADC converter                
    Init_TRIS();                         // Initialize TRIS

    while(1)
    {    
        WAIT_1_SEC();
    }   
}


void Init_ADC (void)
{
    ADCON0 = 0b00000001;                // 00 0000 0 1 Select Channel AN0 (bit 5-2), A/D idle (bit 1) A/D enable (bit 0)
    ADCON1 = 0b00011011;                // Bit3-0 - analog Bit5 - AVss 000\11\011                            
    ADCON2 = 0b11001001;                // Right Justified, 2 tad, fosc/8 (11_001_001)
}

void Init_TRIS (void)
{
    TRISA = 0x0F;                       // Set PORTA pins as inputs
    TRISB = 0x00;                       // Set PORTB pins as outputs
    TRISC = 0x00;                       // Do the same for PORTC
    TRISD = 0x00;
    TRISE = 0x00;
}


void WAIT_1_SEC()
{   SEC_LED = 1;                        // First, turn on the SEC LED
    WAIT_HALF_SEC();                    // Wait for half second (or 500 msec)
    SEC_LED = 0;                        // First, turn on the SEC LED
    WAIT_HALF_SEC();                    // Wait for half second (or 500 msec)
}

void WAIT_HALF_SEC()
{
    T0CON = 0x02;                       // Timer 0, 16-bit mode, prescaler 1:8
    TMR0L = 0xDB;                       // set the lower byte of TMR
    TMR0H = 0xB;                        // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;              // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;               // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);     // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;               // turn off the Timer 0
} 
