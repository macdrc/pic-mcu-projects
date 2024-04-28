
//PART A
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

#define E0 PORTEbits.RE0 //Used in 7SEG upper Function
#define DP PORTDbits.RD7 //Refers to Decimal Point
//Prototype
void putch (char);
void WAIT_1_SEC();
void Select_ADC_Channel(char);
void Init_ADC(void);
void DO_DISPLAY_7SEG_LOWER(char);
void DO_DISPLAY_7SEG_UPPER(char);
void Init_UART();
unsigned int Get_Full_ADC(void);
void Init_TRIS();                                                                 
//Array for BCD
char array[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};  
//Functions before Main
void putch (char c) 
{ 
    while (!TRMT); 
    TXREG = c; 
}
//Wait a Second, Easy to call in Main
void WAIT_1_SEC()                                               
{
    for (int j=0;j<17000;j++);
}
//A2D converter,dependent on desired channel
void Select_ADC_Channel(char channel)                                          
{
    ADCON0 = channel * 4 + 1;
}
//Initialize ADCON 1 and 2
void Init_ADC(void)                                                            
{ 
    ADCON1=0x1A; //19                                                           
    ADCON2=0xA9; //Given value
}
//Lower digit display
void DO_DISPLAY_7SEG_LOWER(char digit)                                         
{
    PORTD = array[digit];
}
//Upper digit display
void DO_DISPLAY_7SEG_UPPER(char digit)                                         
{
    PORTC = array[digit] & 0x3F; 
    char t = array[digit] & 0x40;
    if (t == 0) E0 = 0x00;
    else E0 = 0x01;
}
//Initialize UART communication
void Init_UART() 
{ 
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25); 
    OSCCON = 0x60; 
} 
//#of voltage steps
unsigned int Get_Full_ADC(void)  
{ 
int result;
    ADCON0bits.GO=1; 
    while(ADCON0bits.DONE==1);
    result = (ADRESH * 0x100) + ADRESL; 
    
    return result;
} 
//Initialize TRIS
void Init_TRIS()
{
    TRISA = 0x3F;//5 inputs from RA0 to RA5, An3 Ignored, so 111111 = 0x3F
    TRISB = 0x00;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;
}
//MAIN
void main()
{
    int num_step;
    float voltage_input;
    char U;
    char L;                                             
    Init_UART();
    Init_ADC();                                          
    Init_TRIS();                                           
    while(1)
    {
    Select_ADC_Channel(0);    //Select An0 to read pot. value
    num_step = Get_Full_ADC();
    voltage_input = (num_step * 4.0)/1000; //Get Volt. value          
    U = (int) voltage_input;                                               
    L = (int) ((voltage_input - U)*10);                                     
    printf("The voltage is %.3f \r\n", voltage_input); //Voltage to TerraTerm
    DO_DISPLAY_7SEG_UPPER(U); //Output upper digit of pot. voltage
    DO_DISPLAY_7SEG_LOWER(L); //Output lower digit of pot. voltage
    DP = 0; //Turn on Decimal Point (0 turns on a segment)
    WAIT_1_SEC();
    }
}