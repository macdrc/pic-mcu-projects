#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF

#define D1_RED PORTBbits.RB5
#define D1_GREEN PORTBbits.RB6
#define D1_BLUE PORTBbits.RB7


void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}

void Init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void main()
{
    Init_UART();
    Init_ADC();
    Init_TRIS();

    char LED_7seg[10] = { 0x01, 0x4F, 0x12, 0x06, 0x4C, 0x24, 0x20, 0x0F, 0x00, 0x04};
    
void Init_ADC(void)
{
    ADCON1 = 0b00011011;               // Bit3-0 - analog Bit5 - AVss 000\11\011 
    ADCON0 = 0b00000001;               // Select Channel AN0 (bit 5-2)                               
    ADCON2 = 0b11001001;                // Right Justified, 2 tad, fosc/8 (11_001_001)
}

void Init_TRIS (void)
{
    TRISA = 0x30;                      // Set PORTA pins as inputs
    TRISB = 0x00;                      // Set PORTB pins as outputs
    TRISC = 0x00;                      // Do the same for PORTC
    TRISD = 0x00;
    TRICE = 0x00;
}



    void SET_D1_RED()
    {
        D1_RED = 1;
        D1_GREEN = 0;
        D1_BLUE = 0;
    } 
    
    void SET_D1_GREEN()
    {
        D1_RED = 0;
        D1_GREEN = 1;
        D1_BLUE = 0;
    } 
    
    void SET_D1_BLUE()
    {
        D1_RED = 0;
        D1_GREEN = 0;
        D1_BLUE = 1;
    }
        
    void SET_D1_YELLOW()
    {
        D1_RED = 1;
        D1_GREEN = 1;
        D1_BLUE = 0;
    }
    
    void SET_D1_PURPLE()
    {
        D1_RED = 1;
        D1_GREEN = 0;
        D1_BLUE = 1;
    }
    
    void SET_D1_CYAN()
    {
        D1_RED = 0;
        D1_GREEN = 1;
        D1_BLUE = 1;
    } 

    void SET_D1_WHITE()
    {
        D1_RED = 1;
        D1_GREEN = 1;
        D1_BLUE = 1;
    } 

unsigned int get_full_ADC(void)
{
    int result
    ADCON0bits.GO=1;                        // Start Conversion
    while(ADCON0bits.DONE==1);              // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;     // combine result of upper byte and
                                            // lower byte into result
    return result;                          // return the result.
}  


int convert_F (float Vtemp)
{
    float temp_C;
    int temp_F;
    temp_C = (1035.0 - Vtemp) / 5.50;
    temp_F = (int) (1.8*temp_C + 32);
    return temp_F;
}




    


}
