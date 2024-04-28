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
#define MODE_LED PORTBbits.RB7          // Defines MODE_LED as PORTB bits 7 

#define NS_RED PORTAbits.RA5            // Defines NS_RED as PORTA bits 5
#define NS_GREEN PORTBbits.RB0          // Defines NS_GREEN as PORTB bits 0

#define NSLT_RED PORTBbits.RB4          // Defines NSLT_RED as PORTB bits 4
#define NSLT_GREEN PORTBbits.RB5        // Defines NSLT_GREEN as PORTB bits 5

#define EW_RED PORTBbits.RB1            // Defines EW_RED as PORTB bits 1
#define EW_GREEN PORTBbits.RB2          // Defines EW_GREEN as PORTB bits 2
 
#define EWLT_RED PORTEbits.RE1          // Defines EWLT_RED as PORTE bits 1
#define EWLT_GREEN PORTEbits.RE2        // Defines EWLT_GREEN as PORTE bits 2

#define NSPED_SW PORTAbits.RA1          // Defines NSPED_SW as PORTA bits 1
#define EWPED_SW PORTAbits.RA2          // Defines EWPED_SW as PORTA bits 2
#define NSLT_SW PORTAbits.RA3           // Defines NSLT_SW as PORTA bits 3
#define EWLT_SW PORTAbits.RA4           // Defines EWLT_SW as PORTA bits 4

#define OFF 0                           // OFF is 0
#define RED 1                           // RED is 1
#define GREEN 2                         // GREEM is 2
#define YELLOW 3                        // YELLOW is 3

#define NS_PED 0                        // NS_PED is 0
#define EW_PED 1                        // EW_PED is 1
                                        // BCD array
char array[11] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0xFF};

void Init_ADC();                        // Initiate ADC channels
void Init_TRIS();                       // Initiate TRIS
void init_UART (void);                  // Initiate UART connection

void WAIT_1_SEC();                      // 1 second function
void WAIT_HALF_SEC();                   // 1/2 second function
void WAIT_N_SEC (char seconds);         // N seconds function
void WAIT_1_SEC_W_BEEP();               // Wait 1 second with buzzer

void BCD_Lower_Digit(char digit);       // DISPLAY UPPER DIGIT 7SEG DISPLAY
void BCD_Upper_Digit(char digit);       // DISPLAY LOWER DIGIT 7SEG DISPLAY
void BCD_DP (char dp);                  // DISPLAY DECIMAL POINT

unsigned int get_full_ADC(void);        // Get ADC steps  
void Select_ADC_Channel(char);          // Set ADC channel

void Activate_Buzzer();                 // Activate Buzzer
void Deactivate_Buzzer();               // Deactivate Buzzer

void Set_NS(char color);                // Set NS to color
void Set_NSLT(char color);              // Set NSLT to color
void Set_EW(char color);                // Set EW to color
void Set_EWLT(char color);              // Set EWLT to color

void PED_Control(char Direction, char Num_Sec);
void Day_Mode();
void Night_Mode();


void main(void)                         // Night mode routine
{
    OSCCON = 0x60;                      // set cycles to 1MHz 
    PORTB = 0x00;                       // Clear PortB
    PORTC = 0xFF;                       // Clear PortC                           
    PORTD = 0xFF;                       // Clear PortD                 
    PORTE = 0xFF;                       // Clear PortE
    
    Init_ADC();                         // Initialize ADC converter
    Init_TRIS();                        // Initialize TRIS
    init_UART ();                       // Initiate UART
    Select_ADC_Channel(0);              // Select Channel RA0
    
    while (1)
    {
        int nStep = get_full_ADC ();    // Get ADC steps
        float volts = nStep*5/1024.0;   // Multiply by reference V = 5v
        
        printf ("volts are %0.3f\r\n\n", volts);
        printf ("steps are %d\r\n\n", nStep);
        
        if (volts < 2.570)              // MODE 1 selected
        {
            MODE_LED = 0;               // Turns on the MODE_LED
            Day_Mode();                 // Activates Day_Mode
        }
        else                            // If MODE 0 selected
        {
            MODE_LED = 1;               // Turns off the MODE_LED
            PORTD = 0xFF;               // Turns off 7-seg display
            Night_Mode();               // Activates Night_Mode
        }
    }
}

void Day_Mode()                         // Day mode routine
{
    Set_NSLT (RED);                     // Set NSLT to red
    Set_EW (RED);                       // Set EW to red
    Set_EWLT (RED);                     // Set EWLT to red
    Set_NS (GREEN);                     // Set NS to green

    if (NSPED_SW == 1)                  // If NSPED_SW is 1
    {
        PED_Control(NS_PED, 9);         // Activate NS ped and wait 9 secs
    }
    WAIT_N_SEC (8);                     // Wait 8 seconds
    
    Set_NS (YELLOW);                    // Set NS to yellow
    WAIT_N_SEC (3);                     // Wait 3 seconds
    
    Set_NS (RED);                       // Set NS to red
    
    if (EWLT_SW == 1)                   // If EWLT switch is 1
    {
        Set_EWLT (GREEN);               // Set EWLT to green
        WAIT_N_SEC (7);                 // Wait 7 seconds
        Set_EWLT (YELLOW);              // Set EWLT to yellow
        WAIT_N_SEC (3);                 // Wait 3 seconds
        Set_EWLT (RED);                 // Set EWLT to red
    }
    
    Set_EW (GREEN);                     // Set EW to Green
    
    if (EWPED_SW == 1)                  // If EWPED_SW is 1
    {
        PED_Control(EW_PED, 8);         // Activate NS ped and wait 8 secs
    }
    WAIT_N_SEC (6);                     // Wait 6 seconds
    
    Set_EW (YELLOW);                    // Set EW to yellow
    WAIT_N_SEC (3);                     // Wait 3 seconds
    
    Set_EW (RED);                       // Set EW to red
    
    if (NSLT_SW == 1)                   // If NSLT_SW is 1
    {
        Set_NSLT (GREEN);               // Set NSLT to green
        WAIT_N_SEC (6);                 // Wait 6 seconds
        
        Set_NSLT (YELLOW);              // Set NSLT to yellow
        WAIT_N_SEC (3);                 // Wait 3 seconds
        
        Set_NSLT (RED);                 // Set NSLT to red
    }
    return;
}

void Night_Mode()
{
    Set_NSLT (RED);                     // Set NSLT to red
    Set_EW (RED);                       // Set EW to red
    Set_EWLT (RED);                     // Set EWLT to red
    Set_NS (GREEN);                     // Set NS to green
    
    WAIT_N_SEC (8);                     // Wait 8 seconds    
    Set_NS (YELLOW);                    // Set NS light to yellow
    
    WAIT_N_SEC (2);                     // Wait 2 seconds  
    Set_NS (RED);                       // Set NS light to red
    
    if (EWLT_SW == 1)                   // If EWLT switch is 1
    {
        Set_EWLT (GREEN);               // Set EWLT to green
        WAIT_N_SEC (6);                 // Wait 6 seconds
        Set_EWLT (YELLOW);              // Set EWLT to yellow
        WAIT_N_SEC (2);                 // Wait 2 seconds
        Set_EWLT (RED);                 // Set EWLT to red
    }
    
    Set_EW (GREEN);                     // Set EW to Green
    WAIT_N_SEC (9);                     // Wait 9 seconds
    
    Set_EW (YELLOW);                    // Set EW to yellow
    WAIT_N_SEC (2);                     // Wait 2 seconds
    
    Set_EW (RED);                       // Set EW to red
    
    if (NSLT_SW == 1)                   // If NSLT_SW is 1
    {
        Set_NSLT (GREEN);               // Set NSLT to green
        WAIT_N_SEC (7);                 // Wait 7 seconds
        
        Set_NSLT (YELLOW);              // Set NSLT to yellow
        WAIT_N_SEC (2);                 // Wait 2 seconds
        
        Set_NSLT (RED);                 // Set NSLT to red
    }
    return;
}

void Init_ADC (void)
{
    ADCON0 = 0b00000001;                // 00 0000 0 1 Select Channel AN0 (bit 5-2), A/D idle (bit 1) A/D enable (bit 0)
    ADCON1 = 0b00001110;                // Bit3-0 - analog Bit5 - AVss 000\11\011                            
    ADCON2 = 0xA9;                      // Right Justified, 2 tad, fosc/8 (11_001_001)
}

void Init_TRIS (void)
{
    TRISA = 0x1F;                       // Set PORTA pins as inputs
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
    SEC_LED = 1;
    WAIT_HALF_SEC();
    SEC_LED = 0;
    WAIT_HALF_SEC();
}

void WAIT_HALF_SEC()
{
    T0CON = 0x02;                        // Timer 0, 16-bit mode, prescaler 1:8
    TMR0L = 0xDB;                        // set the lower byte of TMR
    TMR0H = 0xB;                         // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;               // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);      // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                // turn off the Timer 0
} 

void WAIT_N_SEC (char seconds)           // Wait N seconds
{
    char i;
    for (i = 0; i < seconds; i++)
    {
        WAIT_1_SEC();
    }
} 

void WAIT_1_SEC_W_BEEP()
{
    SEC_LED = 1;                         // First, turn on the SEC LED
    Activate_Buzzer();                   // Activate the buzzer
    WAIT_HALF_SEC();                     // Wait for half second (or 500 msec)
    SEC_LED = 0;                         // then turn off the SEC LED
    Deactivate_Buzzer ();                // Deactivate the buzzer
    WAIT_HALF_SEC();                     // Wait for half second (or 500 msec)
}

void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
} 

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0;
}

void BCD_Upper_Digit(char digit)
{
    PORTC = array[digit];
    char Port_E = array[digit] & 0x40;  // 0b 0400 0000
    if  (Port_E == 0) PORTEbits.RE0 = 0x00;                                       
    else PORTEbits.RE0 = 0x01;
}

void BCD_Lower_Digit(char digit)
{
    PORTD = array[digit];
    PORTDbits.RD7 = 1;
}

void BCD_DP (char dp)
{
    PORTDbits.RD7 = ~dp;
}

void PED_Control(char Direction, char Num_Sec)
{
    if (Direction == NS_PED)                       // Routine for NS pedestrian timer
    {
        BCD_Lower_Digit (10);                      // Clear Lower BCD Digit
        for (int i = Num_Sec -1; i > 0; --i)       // Timer starts at n-1
        {                                    
            BCD_Upper_Digit (i);             
            WAIT_1_SEC_W_BEEP();                   // Wait 1 second
        }                                    
        BCD_Upper_Digit (10); 
        WAIT_1_SEC_W_BEEP();
    }                                        
                                             
    else if (Direction == EW_PED)                  // Routine for EW pedestrian crossing
    {                                        
        BCD_Upper_Digit (10);                      // Clear Upper BCD Digit
        for (int i = Num_Sec -1; i > 0; --i)       // Timer starts at n-1
        {
            BCD_Lower_Digit (i);                   // Set Lower to Digit
            WAIT_1_SEC_W_BEEP();                   // Wait 1 second
        }
        BCD_Lower_Digit (10);                      // Clear Lower digit
        WAIT_1_SEC_W_BEEP();
    }
}

void Set_NS(char color)
{
    switch (color)
    {
       case OFF: NS_RED =0;NS_GREEN=0;break;       // Turns off the NS LED
       case RED: NS_RED =1;NS_GREEN=0;break;       // Sets NS LED RED
       case GREEN: NS_RED =0;NS_GREEN=1;break;     // sets NS LED GREEN
       case YELLOW: NS_RED =1;NS_GREEN=1;break;    // sets NS LED YELLOW
    }
} 

void Set_NSLT(char color)
{
    switch (color)
    {
       case OFF: NSLT_RED =0;NSLT_GREEN=0;break;   // Turns off the NSLT LED
       case RED: NSLT_RED =1;NSLT_GREEN=0;break;   // Sets NSLT LED RED
       case GREEN: NSLT_RED =0;NSLT_GREEN=1;break; // sets NSLT LED GREEN
       case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;// sets NSLT LED YELLOW
    }
}

void Set_EW(char color)
{
    switch (color)
    {
       case OFF: EW_RED =0;EW_GREEN=0;break;       // Turns off the EW LED
       case RED: EW_RED =1;EW_GREEN=0;break;       // Sets EW LED RED
       case GREEN: EW_RED =0;EW_GREEN=1;break;     // sets EW LED GREEN
       case YELLOW: EW_RED =1;EW_GREEN=1;break;    // sets EW LED YELLOW
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

void init_UART (void)                               // Init. UART CONNECTION
{
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT 
            & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
} 


 