#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#include "ST7735.h"

#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz


void TIMER1_isr(void);
void INT0_isr(void);
void Initialize_Screen();
void Activate_Buzzer();
void Deactivate_Buzzer();
void wait_one_sec();
unsigned char Nec_state = 0;
unsigned char i,bit_count;
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;

// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK
#define WT               ST7735_WHITE 
#define CY               ST7735_CYAN    

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define OFF 0
#define RED 1
#define GREEN 2
#define BLUE 3
#define MAGENTA 4
#define WHITE 5
#define CYAN 6

#define D1_R PORTDbits.RD0
#define D1_G PORTDbits.RD1
#define D1_B PORTDbits.RD2

#define D2_R PORTDbits.RD5
#define D2_G PORTDbits.RD6
#define D2_B PORTDbits.RD7

#define D3_R PORTAbits.RA3
#define D3_G PORTAbits.RA4
#define D3_B PORTAbits.RA5

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0", "|<<\0", ">>|\0", ">||\0", "VL-\0", "VL+\0", "EQ\0", " 0 \0", "100\0", "200\0", 
                    " 1 \0", " 2 \0" , " 3 \0", " 4 \0", " 5 \0" , " 6 \0", " 7 \0", " 8 \0" , " 9 \0"};
int color_font[21]={WT, WT, WT, WT, WT, WT, WT, WT, WT, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK, BK};
int color[21]={RD, RD, RD, BU, BU, GR, MA, MA, MA, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT};
char d1[21]={RED, RED, RED, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};
char d2[21]={OFF, OFF, OFF, CYAN, CYAN, GREEN, BLUE, BLUE, MAGENTA, WHITE, WHITE, WHITE, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF};
char d3[21]={OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, OFF, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, WHITE};

void putch (char c)
{
    while (!TRMT);
    TXREG = c;
}
void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();
    if (INTCONbits.INT0IF == 1) INT0_isr();
}

void TIMER1_isr(void)
{
    Nec_state = 0;                          // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                   // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                    // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;
}

void clear_d3(void);

void set_d1(char color)
{
    switch(color)
    {
        case OFF: D1_R = 0; D1_G = 0; D1_B = 0;break;
        case RED: D1_R = 1; D1_G = 0; D1_B = 0;break;
        case GREEN: D1_R = 0; D1_G = 1; D1_B = 0;break;
        case BLUE: D1_R = 0; D1_G = 0; D1_B = 1;break;
        case CYAN: D1_R = 0; D1_G = 1; D1_B = 1;break;
        case MAGENTA: D1_R = 1; D1_G = 0; D1_B = 1;break;
        case WHITE: D1_R = 1; D1_G = 1; D1_B = 1;break;
    }
    //clear_d3();
}

void set_d2(char color)
{
    switch(color)
    {
        case OFF: D2_R = 0; D2_G = 0; D2_B = 0;break;
        case RED: D2_R = 1; D2_G = 0; D2_B = 0;break;
        case GREEN: D2_R = 0; D2_G = 1; D2_B = 0;break;
        case BLUE: D2_R = 0; D2_G = 0; D2_B = 1;break;
        case CYAN: D2_R = 0; D2_G = 1; D2_B = 1;break;
        case MAGENTA: D2_R = 1; D2_G = 0; D2_B = 1;break;
        case WHITE: D2_R = 1; D2_G = 1; D2_B = 1;break;
    }
    //clear_d3();
}

void set_d3(char color)
{
    switch(color)
    {
        case OFF: D3_R = 0; D3_G = 0; D3_B = 0;break;
        case RED: D3_R = 1; D3_G = 0; D3_B = 0;break;
        case GREEN: D3_R = 0; D3_G = 1; D3_B = 0;break;
        case BLUE: D3_R = 0; D3_G = 0; D3_B = 1;break;
        case CYAN: D3_R = 0; D3_G = 1; D3_B = 1;break;
        case MAGENTA: D3_R = 1; D3_G = 0; D3_B = 1;break;
        case WHITE: D3_R = 1; D3_G = 1; D3_B = 1;break;
    }
    
}


void INT0_isr(void)
{
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;       // Store Timer1 value
        TMR1H = 0;                          // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_state)
    {
        case 0 :
        {
                                            
            TMR1H = 0;                      // Clear Timer 1 
            TMR1L = 0;                      // Reset Timer1
            PIR1bits.TMR1IF = 0;            //
            T1CON= 0x90;                    // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;           // Enable Timer 1
            bit_count = 0;                  // Force bit count (bit_count) to 0
            Nec_code = 0;                   // Set Nec_code = 0
            Nec_state = 1;                  // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;        // Change Edge interrupt of INT0 to Low to High            
            return;
        }
        
        case 1 :
        {
            if ((Time_Elapsed > 8500) && (Time_Elapsed < 9500))
            {
                Nec_state = 2;
            }
            else 
            {
                force_nec_state0();
                 
            }  
            INTCON2bits.INTEDG0 = 0; 
            return;
        }
        
        case 2 :                            // Add your code here
        {
            if ((Time_Elapsed > 4000) && (Time_Elapsed < 5000))
            {
                Nec_state = 3;
            }
            else 
            {
                force_nec_state0();
               
            }  
            INTCON2bits.INTEDG0 = 1;
            return;            
        }
        
        case 3 :                            // Add your code here
        {
            if ((Time_Elapsed > 400) && (Time_Elapsed < 700))
            {
                Nec_state = 4;
            }
            else 
            {
                force_nec_state0();
              
            }  
            INTCON2bits.INTEDG0 = 0;
            return;
        }
        
        case 4 :                            // Add your code here
        {
            if ((Time_Elapsed > 400) && (Time_Elapsed < 1800))
            {
                Nec_code = Nec_code << 1;
                if (Time_Elapsed > 1000)
                {
                    Nec_code = Nec_code + 1;
                }
                
                bit_count++;
                if (bit_count > 31)   
                {
                    nec_ok = 1;
                    INTCONbits.INT0IE = 0;
                    Nec_state = 0;
                }
                Nec_state = 3;               
            } 
            else 
            {
                force_nec_state0();
               
            }  
            INTCON2bits.INTEDG0 = 1;
            return;
        }
    }
}

void main()
{
    init_UART();
    OSCCON = 0x70;                          // 8 Mhz
    nRBPU = 0;                              // Enable PORTB internal pull up resistor
    TRISA = 0x00;
    TRISB = 0x01;
    TRISC = 0x00;                           // PORTC as output
    TRISD = 0x00;
    TRISE = 0x00;
    ADCON1 = 0x0F;                          //
    Initialize_Screen();
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge H to L
    INTCONbits.INT0IE = 1;                  // Enable external interrupt
    TMR1H = 0;                              // Reset Timer1
    TMR1L = 0;                              //
    PIR1bits.TMR1IF = 0;                    // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                    // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                    // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                     // Enable global interrupts
    nec_ok = 0;                             // Clear flag
    Nec_code = 0x0;                         // Clear code
    
    while(1)
    {
        if (nec_ok == 1)
        {
            nec_ok = 0;

            Nec_code1 = (char) ((Nec_code >> 8));
            printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);
            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
            
            char found = 0xff;
            char led_c = 0xff;
           
            for(int i = 0; i < 21; i++)
            {
                if(Nec_code1 == array1[i])                          
                {
                    found = i;
                    led_c = i;
                }
            } 
            
            set_d1(OFF);
            set_d2(OFF);
            set_d3(OFF);
            
            
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], color_font[found], color[found],TS_1);                  
            }
                
                set_d1(d1[led_c]);
                set_d2(d2[led_c]);
                set_d3(d3[led_c]);
                Activate_Buzzer();
                wait_one_sec();
                Deactivate_Buzzer(); 
                    
        }
    }
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
void wait_one_sec()
{
    for (int k=0; k<17000; k++);
}
void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Fall 22-S1");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 10 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}



