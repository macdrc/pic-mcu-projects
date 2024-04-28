#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#include "I2C_Soft.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "ST7735.h"

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define D1R     0x20

#define D2C     0x06
#define D2G     0x02
#define D2M     0x05
#define D2B     0x04

#define D3W     0xFF

// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK
#define WT               ST7735_WHITE
#define CY               ST7735_CYAN

#define kp               PORTEbits.RE1     

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
char txt1[21][4] = {"CH-\0", "CH \0", "CH+\0", "|<<\0", ">>|\0", ">||\0"," - \0", " + \0", "EQ \0", " 0 \0","100\0", "200\0", " 1 \0", " 2 \0", " 3 \0", " 4 \0", " 5 \0", " 6 \0", " 7 \0", " 8 \0", " 9 \0"};
int color[21]={RD, RD, RD, CY, CY, GR, BU, BU, MA, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT};

char D1[21] = {D1R, D1R, D1R, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char D2[21] = {0, 0, 0, D2C, D2C, D2G, D2B, D2B, D2M, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char D3[21] = {0, 0, 0, 0, 0, 0, 0, 0, 0, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W};


short nec_ok = 0;
//unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;


char tempSecond = 0xff; 
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;


char TempSecond;

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char Nec_code1;
char found;

void Deactivate_Buzzer();
void Activate_Buzzer();
void Initialize_Screen();
void Wait_One_Sec();

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}


void Wait_One_Sec()
{
    for (int k=0;k<17000;k++);
}


void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    init_INTERRUPT();
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    RBPU=0;
    ADCON1= 0x0F;		    // Fill out values
    TRISA = 0x00;
    TRISB = 0b00110001;
    TRISC = 0x00;                   
    TRISD = 0x00;
    TRISE = 0x00;
    
    I2C_Init(100000); 

    DS1621_Init();

}


void main() 
{ 
    Do_Init();                                                  // Initialization
    DS3231_Setup_Time();
    init_INTERRUPT();
    Initialize_Screen();

    while (1)							// This is for the DS1621 testing. Comment this section
    {								// out to move on to the next section

        signed char tempC = DS1621_Read_Temp();
        signed char tempF = (tempC * 9 / 5) + 32;
        //printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
    
//      while (1)						// This is for the DS3231 testing. Comment this section
//      {							// out to move on to the next section
//
        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();
            signed char tempF = (tempC * 9 / 5) + 32;
            printf (" %02x:%02x:%02x %02x/%02x/%02x\r\n\n",hour,minute,second,month,day,year);
           
            printf (" Temperature = %d degreesC = %d degreesF\r\n\n", tempC, tempF);
            printf("------------------------------------------------------------\r\n");
            Wait_One_Sec();
        }
    
        if (nec_ok == 1)					// This is for the final section
        {
            nec_ok = 0;

            //printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);
            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
            
            found = 0xff;
            
            // add code here to look for code
            for(int i = 0;i < 21;i++)
            {
                if(array1[i] == Nec_code1)
                {
                    found = i;
                    if (found == 13){
                    DS3231_Setup_Time();
                    }
                    break;
                }
            }
            
            //printf ("NEC_Code =  %x ", Nec_code1);
            //printf ("Found = %d\r\n", found);
         
            
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_BLACK, color[found],TS_1); 
                
                PORTA = D3[found];
                PORTD = D1[found]|D2[found];
                
                Activate_Buzzer();
                Wait_One_Sec();
                Deactivate_Buzzer();
            }
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

void Initialize_Screen()
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Fall 22-S1");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 11");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}


