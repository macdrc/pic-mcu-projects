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

#define delay 5                     
        
// Define colors to specific bits
#define D1_RED      PORTBbits.RB5        
#define D1_GREEN    PORTBbits.RB6
#define D1_BLUE     PORTBbits.RB7

#define D2_RED      PORTBbits.RB0
#define D2_GREEN    PORTBbits.RB1
#define D2_BLUE     PORTBbits.RB2

#define D3_RED      PORTAbits.RA4
#define D3_GREEN    PORTAbits.RA5


//7 SEG array 
char array[10] = {0x01,0x4F,0x12,0x06,0x4C,0x24,0x20,0x0F,0x00,0x04};
//Prototype
void Init_ADC();                        // Init. A2D 
void init_UART();                       // Init. UART COMMUNCATION 
void Init_Tris();                       // Init. all TRIS 
void WAIT_1_SEC();                      
unsigned int get_full_ADC(void);        // Get voltage steps
void Display_Lower_Digit(char digit);   // DISPLAY UPPER DIGIT 7SEG DISPLAY
void Display_Upper_Digit(char digit);   // DISPLAY LOWER DIGIT 7SEG DISPLAY
void Select_ADC_Channel(char channel);  // Select channel
void Display_D2(int temperature);       // Display D2
void Display_D1(int temperature);       // Display D1 
void Display_D3(int voltage);           // Display D3 

//SET EACH COLOR COMBINATION
void D1_None(void);
void D1_Red(void);
void D1_Green(void);
void D1_Blue(void);
void D1_White(void);

void D2_None(void);
void D2_Red(void);
void D2_Green(void);
void D2_Yellow(void);
void D2_Blue(void);
void D2_Purple(void);
void D2_Cyan(void);
void D2_White(void);

void main(void)
{
    Init_ADC();                     // CALL ADC CONVERTER 
    init_UART();                    
    Init_Tris();                    
    PORTC = 0xFF;                   
    PORTD = 0xFF;                   
    PORTB = 0x00;                   
    
    while(1)
    {    
        Select_ADC_Channel(0);                      //Select channel
        int num_step = get_full_ADC();              // GET NUMSTEP FROM ADC CHANNEL 
        float voltage_mv = num_step*4.0;            // multiply by Ref value (about 4.096)          
        float temp_c = (1035.75 - voltage_mv)/5.575;//calc temp (c))
        float temp_f = (1.80 * temp_c) + 32.0;      // Convert to f
        int intTmpF = (int)temp_f;                    
        char upper = intTmpF /10;                     
        char lower = intTmpF %10;                   
        Display_Upper_Digit(upper);                 // Display upper 
        Display_Lower_Digit(lower);                 // Display lower
        Display_D2(intTmpF);                        
        Display_D1(intTmpF);                       
        
        Select_ADC_Channel(1);                      // Change ADC CHANNEL for photoresistor 
        int num_step2 = get_full_ADC();             // GET LIGHT RESISTOR DATA
        float pvoltage_mv = (num_step2*4.0);        // Calc voltage
        
        Display_D3(pvoltage_mv);                    // Display on D3
        //Display data on TT
        printf("Steps = %d \r\n", num_step);        
        printf("Steps2 = %d \r\n", num_step2);      
        printf("Voltage = %f \r\n", pvoltage_mv);   
        printf("Temperature = %f F \r\n\n", temp_f); 
        WAIT_1_SEC(); 
    }
}

void init_UART(void){ // Init. UART CONNECTION 
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT 
            & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void Select_ADC_Channel(char channel) // Select Channel
{
    ADCON0 = channel * 4 + 1;// shifts bit by 2 to the left, keeps bit 0 on
}                                           

void Init_Tris(){ // Init. TRIS
    TRISA = 0x0F; 
    TRISB = 0x00; 
    TRISC = 0x00; 
    TRISD = 0x00; 
    TRISE = 0x00; 
}
void Init_ADC(void){    // Init. A2D 
    
    ADCON1=0x1B;
    // select pins AN0 through AN3 as analog signal, VDD-VSS as
    // reference voltage
    ADCON2=0xA9;
}
unsigned int get_full_ADC(void){    // Get step data
    int result; 
    ADCON0bits.GO=1;
    // Start Conversion
    while(ADCON0bits.DONE==1);
    // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL;
    // combine result of upper byte and
    // lower byte into result
    return result;
    // return the result.
}
void putch (char c){
    while (!TRMT);
    TXREG = c;
}

void WAIT_1_SEC(){ // WAIT ONE SECOND 
    for(int j = 0; j<17000; j++); 
}





void Display_Upper_Digit(char digit){ // Display upper

    
    PORTC = array[digit] & 0x3F;                                    
    char PE = array[digit] & 0x40;  // 0100 0000                                   
    if (PE == 0) PORTE = 0x00;                                       
    else PORTE = 0x01;                                                 
}

void Display_Lower_Digit(char digit){// Display lower
    PORTD = array[digit]; 
    
}

void Display_D1(int temperature){ // Check temp, display to D1
    if(temperature <= 45) D1_None();
    else if(temperature >= 46 && temperature<= 55) D1_Red(); 
    else if(temperature >= 56 && temperature<= 65) D1_Green(); 
    else if(temperature >= 66 && temperature <= 75) D1_Blue(); 
    else if(temperature >= 76) D1_White();
}

void Display_D2(int temperature){// Check temp, display to D2

 if (temperature > 70) D2_White();
    
    else if (temperature <=0) D2_None();
    else  (PORTB = temperature/10);   
}                                     

void Display_D3(int voltage){ //Check volatge, diplay on D3
    if(voltage < 2500)
        PORTA = 0x01<<4;                  
    else if(voltage >= 2500 && voltage < 3400)
        PORTA = 0x02<<4; 
    else 
        PORTA = 0x03<<4;
    
    
    WAIT_1_SEC();
}



// COLOR COMBINATIONS FOR LEDS D1 - D2
void D1_None(void)
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 0;
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

void D1_Blue(void)
{
    D1_RED = 0;
    D1_GREEN = 0;
    D1_BLUE = 1;
}

void D1_White(void)
{
    D1_RED = 1;
    D1_GREEN = 1;
    D1_BLUE = 1;
}

void D2_None(void)
{
    D2_RED = 0;
    D2_GREEN = 0;
    D2_BLUE = 0;
}

void D2_White(void)
{
    D2_RED = 1;
    D2_GREEN = 1;
    D2_BLUE = 1;
}