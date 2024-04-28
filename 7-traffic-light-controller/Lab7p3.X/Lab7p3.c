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
#pragma config CCP2MX = PORTBE

#define SEC_LED PORTDbits.RD7           // Defines SEC_LED as PORTD bits 7

#define NS_RED PORTAbits.RA5            // Defines NS_RED as PORTA bits 5
#define NS_GREEN PORTBbits.RB0          // Defines NS_GREEN as PORTB bits 0

#define NSLT_RED PORTBbits.RB4          // Defines NSLT_RED as PORTB bits 4
#define NSLT_GREEN PORTBbits.RB5        // Defines NSLT_GREEN as PORTB bits 5

#define EW_RED PORTBbits.RB1            // Defines EW_RED as PORTB bits 1
#define EW_GREEN PORTBbits.RB2          // Defines EW_GREEN as PORTB bits 2
 
#define EWLT_RED PORTEbits.RE1        // Defines EWLT_RED as PORTE bits 1
#define EWLT_GREEN PORTEbits.RE2        // Defines EWLT_GREEN as PORTE bits 2

#define OFF 0                           // OFF is 0
#define RED 1                           // RED is 1
#define GREEN 2                         // GREEM is 2
#define YELLOW 3                        // YELLOW is 3


void Init_ADC();                        // Initiate ADC channels
void Init_TRIS();                       // Initiate TRIS

void WAIT_1_SEC();                      // 1 second function
void WAIT_HALF_SEC();                   // 1/2 second function
void Wait_N_Seconds (char seconds);     // N seconds function

void BCD_Lower_Digit(char digit);       // DISPLAY UPPER DIGIT 7SEG DISPLAY
void BCD_Upper_Digit(char digit);       // DISPLAY LOWER DIGIT 7SEG DISPLAY
void BCD_DP (char dp);                  // DISPLAY DECIMAL POINT

unsigned int get_full_ADC(void);        // Get ADC steps  
void Select_ADC_Channel(char);          // Set ADC channel

void Activate_Buzzer();                 // Activate Buzzer
void Deactivate_Buzzer();               // Deactivate Buzzer

void Set_NS(char color);
void Set_NSLT(char color);
void Set_EW(char color);
void Set_EWLT(char color);


void main(void)
{
    OSCCON = 0x60;                      // set cycles to 1MHz 
    PORTB = 0x00;                       // Clear PortB
    PORTC = 0xFF;                       // Clear PortC                           
    PORTD = 0xFF;                       // Clear PortD                 
    PORTE = 0xFF;                       // Clear PortE
    
    Init_ADC();                         // Initialize ADC converter
    Init_TRIS();                        // Initialize TRIS    
    Select_ADC_Channel(0);              // Select Channel RA0
    
    while (1)
    {
        get_full_ADC ();
         for (int i=0; i<4; i++)
        {
            Set_NS(i); // Set color for North-South direction
            Set_NSLT(i); // Set color for North-South Left-Turn direction
            Set_EW(i); // Set color for East-West direction
            Set_EWLT(i); // Set color for East-West Left-Turn direction
            Wait_N_Seconds(1); // call Wait-N-Second routine to wait for 1 second
        } 
        
    }
}


void Init_ADC (void)
{
    ADCON0 = 0b00000001;                // 00 0000 0 1 Select Channel AN0 (bit 5-2), A/D idle (bit 1) A/D enable (bit 0)
    ADCON1 = 0b00011010;                // Bit3-0 - analog Bit5 - AVss 000\11\011                            
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

void Select_ADC_Channel(char channel)   
{
    ADCON0 = channel * 4 + 1;           // shifts bit by 2 to the left, keeps bit 0 on
}   

unsigned int get_full_ADC (void)        // Get ADC steps from input
{
    int result;
    ADCON0bits.GO=1;                    // Start Conversion
    while(ADCON0bits.DONE==1);          // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL; // combine result of upper byte and
                                        // lower byte into result
    return result;                      // return the result.
} 

void WAIT_1_SEC()
{
    WAIT_HALF_SEC();
    WAIT_HALF_SEC();
}

void WAIT_HALF_SEC()
{
    T0CON = 0x02;                          // Timer 0, 16-bit mode, prescaler 1:8
    TMR0L = 0xDB;                          // set the lower byte of TMR
    TMR0H = 0xB;                           // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                 // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                  // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);        // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                  // turn off the Timer 0
} 

void Wait_N_Seconds (char seconds)      // Wait N seconds
{
    char i;
    for (i = 0; i < seconds; i++)
    {
        WAIT_1_SEC();
    }
} 

void Set_NS(char color)
{
    switch (color)
    {
       case OFF: NS_RED =0;NS_GREEN=0;break;   // Turns off the NS LED
       case RED: NS_RED =1;NS_GREEN=0;break;   // Sets NS LED RED
       case GREEN: NS_RED =0;NS_GREEN=1;break; // sets NS LED GREEN
       case YELLOW: NS_RED =1;NS_GREEN=1;break;// sets NS LED YELLOW
    }
} 

void Set_NSLT(char color)
{
    switch (color)
    {
       case OFF: NSLT_RED =0;NSLT_GREEN=0;break;   // Turns off the NS LED
       case RED: NSLT_RED =1;NSLT_GREEN=0;break;   // Sets NS LED RED
       case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; // sets NS LED GREEN
       case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;// sets NS LED YELLOW
    }
}

void Set_EW(char color)
{
    switch (color)
    {
       case OFF: EW_RED =0;EW_GREEN=0;break;   // Turns off the NS LED
       case RED: EW_RED =1;EW_GREEN=0;break;   // Sets NS LED RED
       case GREEN: EW_RED =0;EW_GREEN=1;break; // sets NS LED GREEN
       case YELLOW: EW_RED =1;EW_GREEN=1;break;// sets NS LED YELLOW
    }
} 

void Set_EWLT(char color)
{
    switch (color)
    {
       case OFF: EWLT_RED =0;EWLT_GREEN=0;break;   // Turns off the NS LED
       case RED: EWLT_RED =1;EWLT_GREEN=0;break;   // Sets NS LED RED
       case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; // sets NS LED GREEN
       case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;// sets NS LED YELLOW
    }
} 


 