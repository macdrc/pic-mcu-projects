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

#define D1_RED      PORTBbits.RB4    // Define colors to specific bits   
#define D1_GREEN    PORTBbits.RB5
#define D1_BLUE     PORTBbits.RB6

#define D2_RED      PORTBbits.RB0
#define D2_GREEN    PORTBbits.RB1
#define D2_BLUE     PORTBbits.RB2
                                    // BCD array
char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
                                    // LED D1 array
char D2_array[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

void Init_ADC();                    // Initiate ADC channels
void init_UART();                   // Initiate UART
void Init_TRIS();                   // Initiate TRIS

void WAIT_1_SEC();                  // 1 second function
void WAIT_HALF_SEC();               // 1/2 second function
void BCD_Lower_Digit(char digit);   // DISPLAY UPPER DIGIT 7SEG DISPLAY
void BCD_Upper_Digit(char digit);   // DISPLAY LOWER DIGIT 7SEG DISPLAY
void BCD_DP (char dp);              // DISPLAY DECIMAL POINT
unsigned int get_full_ADC(void);    // Get ADC steps  
void Select_ADC_Channel(char);      // Set ADC channel
void Activate_Buzzer();             // Activate Buzzer
void Deactivate_Buzzer();           // Deactivate Buzzer

void Display_D1(int temperature);   // Display D1
void Display_D2(int temperature);   // Display D2
void Display_D3(int voltage);       // Display D3 

void D1_None(void);                 // D1 Colors
void D1_Yellow(void);

void D2_None(void);                 // D2 Colors
void D2_Red(void);                  
void D2_Green(void);
void D2_Yellow(void);
void D2_Blue(void);
void D2_Purple(void);
void D2_Cyan(void);
void D2_White(void);

void main(void)
{
    OSCCON = 0x60;                  // Set clock to 1MHz so timer will be accurate
    
    PORTB = 0x00;                   // Clear PortB
    PORTC = 0xFF;                   // Clear PortC                           
    PORTD = 0xFF;                   // Clear PortD                 
    PORTE = 0xFF;                   // Clear PortE
    
    Init_ADC();                     // Initialize ADC converter
    init_UART();                    // Initialize UART
    Init_TRIS();                    // Initialize TRIS
    
    Select_ADC_Channel(4);          // Select input channel AN4 RA5
    
    while(1)
    {   
        int num_step = get_full_ADC();                      // Get step values from input channel
        float voltage_v = (num_step * 4)/1000.0;            // Multiply by reference voltage and change to volts
        float R_ref_voltage = (4.096 - voltage_v);          // Subtract measured voltage from ref resistor
        float R_ref_current = R_ref_voltage / 98400;        // Divide ref voltage by ref resistance of 983 ohms
        float R_test_float = (voltage_v / R_ref_current);   // Find R test ohms by dividing test voltage by current
        long R_test = (long) R_test_float;                  // Change the variable to long for size
    
        if (R_test < 10000)                                 // Check if less than 10000
        {
            BCD_Upper_Digit (R_test/1000);                  // Set upper digit
            BCD_Lower_Digit (R_test/100%10);                // Set lower digit
            BCD_DP (1);                                     // Set decimal point
            PORTB = 0x00;                                   // Clear PortB leftover bits 
        }
        else if (R_test > 10000, R_test < 100000)           // Check between 10000 and 100000        
        {
            BCD_Upper_Digit (R_test/10000);                 // Set upper BCD digit
            BCD_Lower_Digit ((R_test/1000)%10);             // Set lower BCD digit
            BCD_DP (0);                                     // Set decimal point
            PORTB = D2_array[R_test/10000];                 // Set D2 LED to follow array
        } 
        if (R_test > 70000)                                 // Check if less than 70000
        {
            PORTB = D2_array[7];                            // Set D2 array
        }
        if (R_test > 100000)                                // Check if more than 100000
        {
            BCD_Upper_Digit (0);                            // Set upper digit
            PORTD = 0x47;                                   // Set BCD to display "OL"
            PORTDbits.RD7 = 1;                              // Set BCD to display "OL"
            PORTB = D2_array[7];                            // Set LED to White
        }
        if (R_test < 70)                                    // Check if less than 70
            {
                Activate_Buzzer();                          // Activate buzzer if less
                D1_Yellow();                                // Activate yellow led if less
            }
        else 
            {
                Deactivate_Buzzer();                        // Else deactivate buzzer
            }
        printf ("The Resistance is %li\r\n\n\n", R_test);   // Print to Teraterm
        WAIT_HALF_SEC();                                    // Wait half second
    }   
}
void Init_ADC (void)
{
    ADCON0 = 0b00000001;               // 00 0000 0 1 Select Channel AN0 (bit 5-2), A/D idle (bit 1) A/D enable (bit 0)
    ADCON1 = 0b00011011;               // Bit3-0 - analog Bit5 - AVss 000\11\011                            
    ADCON2 = 0b11001001;               // Right Justified, 2 tad, fosc/8 (11_001_001)
}

void Init_TRIS (void)
{
    TRISA = 0x0F;                      // Set PORTA pins as inputs
    TRISB = 0x00;                      // Set PORTB pins as outputs
    TRISC = 0x00;                      // Do the same for PORTC
    TRISD = 0x00;                      // Do the same for PORTD
    TRISE = 0x00;                      // Do the same for PORTE
}

void init_UART (void)                  // Init. UART CONNECTION
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
    T0CON = 0x02;                       // Timer 0, 16-bit mode, prescaler 1:8
    TMR0L = 0xDB;                       // set the lower byte of TMR
    TMR0H = 0xB;                        // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;              // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;               // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);     // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;               // turn off the Timer 0
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

//BCD DIGITS-------------------------------//
void BCD_Upper_Digit(char digit)
{
    PORTC = array[digit];
    char Port_E = array[digit] & 0x40;
    if  (Port_E == 0) PORTE = 0x00;                                       
    else PORTE = 0x01;
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

//LED COLORS-------------------------------//
void D1_None(void)
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 0;
}
void D1_Red(void)
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;
}

void D1_Green(void)
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 0;
}

void D1_Yellow(void)
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 0;
}

void D1_Blue(void)
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;
}

void D1_Purple(void)
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 1;
}

void D1_Cyan(void)
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 1;
}

void D1_White(void)
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}

void D2_None(void)
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 0;
}
void D2_Red(void)
{
    D1_RED = 1;
    D1_GREEN = 0;
    D1_BLUE = 0;
}

void D2_Green(void)
{
    D1_RED = 0;
    D1_GREEN = 1;
    D1_BLUE = 0;
}

void D2_Yellow(void)
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 0;
}

void D2_Blue(void)
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;
}

void D2_Purple(void)
{
    D2_RED = 1;
    D2_GREEN = 0;
    D2_BLUE = 1;
}

void D2_Cyan(void)
{
    D2_RED = 0;
    D2_GREEN = 1;
    D2_BLUE = 1;
}

void D2_White(void)
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}
