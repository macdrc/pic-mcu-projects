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
#pragma config CCP2MX = PORTBE


#define _XTAL_FREQ      8000000         // Set operation for 8 Mhz
#define TMR_CLOCK       _XTAL_FREQ/4    // Timer Clock 2 Mhz

#define TFT_DC PORTCbits.RC2            // Location of TFT D/C
#define TFT_CS PORTCbits.RC4            // Location of TFT Chip Select
#define TFT_RST PORTCbits.RC1           // Location of TFT Reset


#define SEC_LED PORTDbits.RD7           // Defines SEC_LED as PORTD bits 7
#define MODE_LED PORTBbits.RB7          // Defines MODE_LED as PORTB bits 7 

#define NS_RED PORTAbits.RA1            // Defines NS_RED as PORTA bits 5
#define NS_GREEN PORTAbits.RA2          // Defines NS_GREEN as PORTB bits 0

#define NSLT_RED PORTBbits.RB4          // Defines NSLT_RED as PORTB bits 4
#define NSLT_GREEN PORTBbits.RB5        // Defines NSLT_GREEN as PORTB bits 5

#define EW_RED PORTAbits.RA5            // Defines EW_RED as PORTB bits 1
#define EW_GREEN PORTBbits.RB6          // Defines EW_GREEN as PORTB bits 2
 
#define EWLT_RED PORTEbits.RE1          // Defines EWLT_RED as PORTE bits 1
#define EWLT_GREEN PORTEbits.RE2        // Defines EWLT_GREEN as PORTE bits 2

//#define NSPED_SW PORTBbits.RB0          // Defines NSPED_SW as PORTA bits 1
//#define EWPED_SW PORTBbits.RB1          // Defines EWPED_SW as PORTA bits 2
char NSPED_SW;
char EWPED_SW;

#define NSLT_SW PORTAbits.RA3           // Defines NSLT_SW as PORTA bits 3
#define EWLT_SW PORTAbits.RA4           // Defines EWLT_SW as PORTA bits 4

#define SW4 PORTBbits.RB2               // Defines button sw4

#define OFF 0                           // OFF is 0
#define RED 1                           // RED is 1
#define GREEN 2                         // GREEM is 2
#define YELLOW 3                        // YELLOW is 3

#define NS_PED 0                        // NS_PED is 0
#define EW_PED 1                        // EW_PED is 1


// colors
#define OFF 		0               // Defines OFF as decimal value 0
#define RED 		1               // Defines RED as decimal value 1
#define GREEN 		2               // Defines GREEN as decimal value 2
#define YELLOW 		3               // Defines YELLOW as decimal value 3

#define Circle_Size     7               // Size of Circle for Light
#define Circle_Offset   15              // Location of Circle
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of PED Text
#define Count_Offset    10              // Location of Count

#define XTXT            30              // X location of Title Text 
#define XRED            40              // X location of Red Circle
#define XYEL            60              // X location of Yellow Circle
#define XGRN            80              // X location of Green Circle
#define XCNT            100             // X location of Sec Count

#define EW              0               // Number definition of East/West
#define EWLT            1               // Number definition of East/West Left Turn
#define NS              2               // Number definition of North/South
#define NSLT            3               // Number definition of North/South Left Turn
 
#define Color_Off       0               // Number definition of Off Color
#define Color_Red       1               // Number definition of Red Color
#define Color_Green     2               // Number definition of Green Color
#define Color_Yellow    3               // Number definition of Yellow Color

#define EW_Txt_Y        20
#define EW_Cir_Y        EW_Txt_Y + Circle_Offset
#define EW_Count_Y      EW_Txt_Y + Count_Offset
#define EW_Color        ST7735_CYAN

#define EWLT_Txt_Y      50
#define EWLT_Cir_Y      EWLT_Txt_Y + Circle_Offset
#define EWLT_Count_Y    EWLT_Txt_Y + Count_Offset
#define EWLT_Color      ST7735_WHITE

#define NS_Txt_Y        80
#define NS_Cir_Y        NS_Txt_Y + Circle_Offset
#define NS_Count_Y      NS_Txt_Y + Count_Offset
#define NS_Color        ST7735_BLUE 

#define NSLT_Txt_Y      110
#define NSLT_Cir_Y      NSLT_Txt_Y + Circle_Offset
#define NSLT_Count_Y    NSLT_Txt_Y + Count_Offset
#define NSLT_Color      ST7735_MAGENTA

#define PED_EW_Count_Y  30
#define PED_NS_Count_Y  90
#define PED_Count_X     2
#define Switch_Txt_Y    140


#include "ST7735_TFT.c"


char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char EW_Count[]     = "00";             // text storage for EW Count
char EWLT_Count[]   = "00";             // text storage for EW Left Turn Count
char NS_Count[]     = "00";             // text storage for NS Count
char NSLT_Count[]   = "00";             // text storage for NS Left Turn Count

char PED_EW_Count[] = "00";             // text storage for EW Pedestrian Count
char PED_NS_Count[] = "00";             // text storage for NS Pedestrian Count

char SW_EWPED_Txt[] = "0";              // text storage for EW Pedestrian Switch
char SW_EWLT_Txt[]  = "0";              // text storage for EW Left Turn Switch
char SW_NSPED_Txt[] = "0";              // text storage for NS Pedestrian Switch
char SW_NSLT_Txt[]  = "0";              // text storage for NS Left Turn Switch

char SW_MODE_Txt[]  = "D";              // text storage for Mode Light Sensor
	
char Act_Mode_Txt[]  = "D";             // text storage for Actual Mode
char FlashingS_Txt[] = "0";             // text storage for Emergency Status
char FlashingR_Txt[] = "0";             // text storage for Flashing Request

char dir;
char Count;                             // RAM variable for Second Count
char PED_Count;                         // RAM variable for Second Pedestrian Count

char SW_EWPED;                          // RAM variable for EW Pedestrian Switch
char SW_EWLT;                           // RAM variable for EW Left Turn Switch
char SW_NSPED;                          // RAM variable for NS Pedestrian Switch
char SW_NSLT;                           // RAM variable for NS Left Turn Switch

char SW_MODE;                           // RAM variable for Mode Light Sensor
int MODE;
char direction;
float volts;

int INT0_flag, INT1_flag, INT2_flag = 0;

char FLASHING_REQUEST;
int FLASHING;
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


void update_LCD_color(char direction, char color);
void update_LCD_PED_Count(char direction, char count);
void Initialize_Screen(void);
void update_LCD_misc(void);
void update_LCD_count(char, char);

void Do_Init();

void INT0_ISR();
void INT1_ISR();
void INT2_ISR();
void interrupt high_priority chkisr();
void Do_Flashing();

void Initialize_Screen()
{
  LCD_Reset();
  TFT_GreenTab_Initialize();
  fillScreen(ST7735_BLACK);
  
  /* TOP HEADER FIELD */
  txt = buffer;
  strcpy(txt, "ECE3301L Fall 22-S1");  
  drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* MODE FIELD */
  strcpy(txt, "Mode:");
  drawtext(2, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

  /* EMERGENCY REQUEST FIELD */
  strcpy(txt, "FR:");
  drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  /* EMERGENCY STATUS FIELD */
  strcpy(txt, "FS:");
  drawtext(80, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
  
  /* SECOND UPDATE FIELD */
  strcpy(txt, "*");
  drawtext(120, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
      
  /* EAST/WEST UPDATE FIELD */
  strcpy(txt, "EAST/WEST");
  drawtext  (XTXT, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EW_Cir_Y-8, 60, 18, EW_Color);
  fillCircle(XRED, EW_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EW_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EW_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);

  /* EAST/WEST LEFT TURN UPDATE FIELD */
  strcpy(txt, "E/W LT");
  drawtext  (XTXT, EWLT_Txt_Y, txt, EWLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, EWLT_Cir_Y-8, 60, 18, EWLT_Color);  
  fillCircle(XRED, EWLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, EWLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, EWLT_Cir_Y, Circle_Size, ST7735_GREEN);   
  drawtext  (XCNT, EWLT_Count_Y, EWLT_Count, EWLT_Color, ST7735_BLACK, TS_2);

  /* NORTH/SOUTH UPDATE FIELD */
  strcpy(txt, "NORTH/SOUTH");
  drawtext  (XTXT, NS_Txt_Y  , txt, NS_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NS_Cir_Y-8, 60, 18, NS_Color);
  drawCircle(XRED, NS_Cir_Y  , Circle_Size, ST7735_RED);
  drawCircle(XYEL, NS_Cir_Y  , Circle_Size, ST7735_YELLOW);
  fillCircle(XGRN, NS_Cir_Y  , Circle_Size, ST7735_GREEN);
  drawtext  (XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* NORTH/SOUTH LEFT TURN UPDATE FIELD */
  strcpy(txt, "N/S LT");
  drawtext  (XTXT, NSLT_Txt_Y, txt, NSLT_Color, ST7735_BLACK, TS_1);
  drawRect  (XTXT, NSLT_Cir_Y-8, 60, 18, NSLT_Color);
  fillCircle(XRED, NSLT_Cir_Y, Circle_Size, ST7735_RED);
  drawCircle(XYEL, NSLT_Cir_Y, Circle_Size, ST7735_YELLOW);
  drawCircle(XGRN, NSLT_Cir_Y, Circle_Size, ST7735_GREEN);  
  drawtext  (XCNT, NSLT_Count_Y, NSLT_Count, NSLT_Color, ST7735_BLACK, TS_2);
  

  /* EAST/WEST PEDESTRIAM UPDATE FIELD */  
  drawtext(2, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);
  strcpy(txt, "PEW");  
  drawtext(3, EW_Txt_Y, txt, EW_Color, ST7735_BLACK, TS_1);

  /* NORTH/SOUTH PEDESTRIAM UPDATE FIELD */
  strcpy(txt, "PNS");  
  drawtext(3, NS_Txt_Y, txt, NS_Color, ST7735_BLACK, TS_1);
  drawtext(2, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);
    
  /* MISCELLANEOUS UPDATE FIELD */  
  strcpy(txt, "EWP EWLT NSP NSLT MD");
  drawtext(1,  Switch_Txt_Y, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(6,  Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(32, Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(58, Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  drawtext(87, Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
  
  drawtext(112,Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
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



void main(void)                         // Night mode routine
{
    Init_ADC();                         // Initialize ADC converter
    Init_TRIS();                        // Initialize TRIS
    init_UART ();                       // Initiate UART
    Select_ADC_Channel(0);              // Select Channel RA0
    
    OSCCON = 0x70;                      // set cycles to 1MHz 
    RBPU = 0;   
    Initialize_Screen(); 
    Do_Init();
    
    while (1)
    {
        int nStep = get_full_ADC ();    // Get ADC steps
        volts = nStep*4/1024.0;   // Multiply by reference V = 5v
        
        if (volts < 2.3)              // MODE 1 selected
        {
            Day_Mode();                 // Activates Day_Mode
            
        }
        else                            // If MODE 0 selected
        {
            Night_Mode();               // Activates Night_Mode
        }
        if (FLASHING_REQUEST == 1)
        {
            Do_Flashing();
        }
        else
        {
            FLASHING_REQUEST = 0;
        }
    }
    
}

void Day_Mode()                         // Day mode routine
{                                       // turns on the MODE_LED
    MODE_LED = 0;
    Act_Mode_Txt[0] = 'D';
    MODE = 1;
    while(1)
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
            PED_Control(EW_PED, 8);         // Activate EW ped and wait 8 secs
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
    break;
    }
}

void Night_Mode()
{                                 // turns on the MODE_LED
    MODE_LED = 1;
    Act_Mode_Txt[0] = 'N';
    MODE = 0;
    
    EWPED_SW = 0;
    NSPED_SW = 0;

    
    while(1)
    {
        Set_NSLT (RED);                 // Set NSLT to red
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
        break;
    }
    
}

void Init_ADC (void)
{
    ADCON0 = 0b00000001;                // 00 0000 0 1 Select Channel AN0 (bit 5-2), A/D idle (bit 1) A/D enable (bit 0)
    ADCON1 = 0b00001110;                // Bit0 - analog Bit5 - AVss 000\11\011                            
    ADCON2 = 0xA9;                      // Right Justified, 2 tad, fosc/8 (11_001_001)
}

void Init_TRIS (void)
{
    TRISA = 0b00011001;                       // Set PORTA pins as inputs
    TRISB = 0b00000111;                       // Set PORTB pins as outputs
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
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    
    WAIT_HALF_SEC();
    
    SEC_LED = 0;
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    
    WAIT_HALF_SEC();
    update_LCD_misc();
}

void WAIT_HALF_SEC()
{
    T0CON = 0x03;                        // Timer 0, 16-bit mode, prescaler 1:8
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
    for (i = seconds; i > 0; i--)
    {
        update_LCD_count(direction, i);
        WAIT_1_SEC();
    }
        update_LCD_count(direction, 0);
   
} 

void WAIT_1_SEC_W_BEEP()
{
    SEC_LED = 1;                         // First, turn on the SEC LED
    Activate_Buzzer();                   // Activate the buzzer
    strcpy(txt,"*");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
    WAIT_HALF_SEC();                     // Wait for half second (or 500 msec)
    
    
    SEC_LED = 0;                         // then turn off the SEC LED
    Deactivate_Buzzer ();                // Deactivate the buzzer
    strcpy(txt," ");
    drawtext(120,10,txt,ST7735_WHITE,ST7735_BLACK,TS_1);
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

void PED_Control(char Direction, char Num_Sec)
{
for(char i = Num_Sec-1; i>0; i--)
    {
        update_LCD_PED_Count(direction, i);
        WAIT_1_SEC_W_BEEP();                // hold the number on 7-Segment for 1 second
    }
        update_LCD_PED_Count(direction, 0);     // leaves the 7-Segment off for 1 second                     // Clear Lower digit
        WAIT_1_SEC_W_BEEP();
        
        if (NSPED_SW == 1)
        {
            NSPED_SW = 0;
        }
        else if (EWPED_SW == 1)
        {
            EWPED_SW = 0;
        }
}

void Set_NS(char color)
{
    direction = NS;
    update_LCD_color(direction, color);
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
    direction = NSLT;			// Add these two lines here
    update_LCD_color(direction, color);    
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
    direction = EW;			// Add these two lines here
    update_LCD_color(direction, color);
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
    direction = EWLT;			// Add these two lines here
    update_LCD_color(direction, color);
    switch (color)
    {
       case OFF: EWLT_RED =0;EWLT_GREEN=0;break;   // Turns off the NS LED
       case RED: EWLT_RED =1;EWLT_GREEN=0;break;   // Sets NS LED RED
       case GREEN: EWLT_RED =0;EWLT_GREEN=1;break; // sets NS LED GREEN
       case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;// sets NS LED YELLOW
    }
} 

 void update_LCD_color(char direction, char color)
{
    char Circle_Y;
    Circle_Y = EW_Cir_Y + direction * 30;    
    
    if (color == Color_Off)     //if Color off make all circles black but leave outline
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);                       
    }    
    
    if (color == Color_Red)     //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }

    if (color == Color_Green)     //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
    
        if (color == Color_Yellow)     //if the color is red only fill the red circle with red
    {
        fillCircle(XRED, Circle_Y, Circle_Size, ST7735_BLACK);
        fillCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        fillCircle(XGRN, Circle_Y, Circle_Size, ST7735_BLACK); 
        drawCircle(XRED, Circle_Y, Circle_Size, ST7735_RED);            
        drawCircle(XYEL, Circle_Y, Circle_Size, ST7735_YELLOW);
        drawCircle(XGRN, Circle_Y, Circle_Size, ST7735_GREEN);  
    }
          
    // put code here
}

void update_LCD_count(char direction, char count)
{
   PORTD = (PORTD & 0x80) | array[count];
   switch (direction)                   //update traffic light no ped time
   {
      case EW:        
        EW_Count[0] = count/10  + '0';
        EW_Count[1] = count%10  + '0';
        drawtext(XCNT, EW_Count_Y, EW_Count, EW_Color, ST7735_BLACK, TS_2);                
        break;
  
      case NS:        
        NS_Count[0] = count/10  + '0';
        NS_Count[1] = count%10  + '0';
        drawtext(XCNT, NS_Count_Y, NS_Count, NS_Color, ST7735_BLACK, TS_2);                
        break;
        
      case EWLT:                        // Change count for EWLT if EWLT is the current light
        EWLT_Count[0] = count/10 + '0';
        EWLT_Count[1] = count%10 + '0';
        drawtext(XCNT, EWLT_Count_Y, EWLT_Count,
        EWLT_Color, ST7735_BLACK, TS_2);
        break;
        
      case NSLT: // Change count for NSLT if NSLT is the current light
        NSLT_Count[0] = count/10 + '0';
        NSLT_Count[1] = count%10 + '0';
        drawtext(XCNT, NSLT_Count_Y, NSLT_Count,
        NSLT_Color, ST7735_BLACK, TS_2);
        break;
    }  
}

void update_LCD_PED_Count(char direction, char count)
{
   PORTD = (PORTD & 0x80) | array[count];
   switch (direction)
   {
      case EW:       
        PED_EW_Count[0] = count/10  + '0';          // PED count upper digit
        PED_EW_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_EW_Count_Y, PED_EW_Count, EW_Color, ST7735_BLACK, TS_2);        
        break;

      case NS:       
        PED_NS_Count[0] = count/10  + '0';          // PED count upper digit
        PED_NS_Count[1] = count%10  + '0';          // PED Lower
        drawtext(PED_Count_X, PED_NS_Count_Y, PED_NS_Count, NS_Color, ST7735_BLACK, TS_2);        
        break;        
   }
   
}

void update_LCD_misc()
{
        int nStep = get_full_ADC ();    // Get ADC steps
        float volts = nStep*5/1024.0;   // Multiply by reference V = 5v
        
        printf ("volts are %0.3f\r\n\n", volts);

        
        if (volts < 2.570)              // MODE 1 selected
        {
            SW_MODE = 1;
        }
        else                            // If MODE 0 selected
        {
            SW_MODE = 0;                // Activates Night_Mode
        }

    
    SW_EWPED = EWPED_SW;
    SW_EWLT = EWLT_SW;    
    SW_NSPED = NSPED_SW;
    SW_NSLT = NSLT_SW;

    if (SW_MODE == 0) SW_MODE_Txt[0] = 'N'; else SW_MODE_Txt[0] = 'D';      // Set Text at bottom of screen to switch states
    if (SW_NSPED == 0) SW_NSPED_Txt[0] = '0'; else SW_NSPED_Txt[0] = '1';      // Set Text at bottom of screen to switch states
    if (SW_NSLT == 0) SW_NSLT_Txt[0] = '0'; else SW_NSLT_Txt[0] = '1';
    if (SW_EWPED == 0) SW_EWPED_Txt[0] = '0'; else SW_EWPED_Txt[0] = '1'; 
    if (SW_EWLT == 0) SW_EWLT_Txt[0] = '0'; else SW_EWLT_Txt[0] = '1';
    if (FLASHING_REQUEST == 0)FlashingR_Txt[0] = '0'; else FlashingR_Txt[0] = '1';
    if (FLASHING == 0)FlashingS_Txt[0] = '0'; else FlashingS_Txt[0] = '1';
    // FlashingS_Txt[0]='0', FlashingR_Txt[0]='1'
           // FlashingS_Txt[0]='1', FlashingR_Txt[0]='0'
    
    
    drawtext(35,10, Act_Mode_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);    
    drawtext(6,   Switch_Txt_Y+9, SW_EWPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(32,  Switch_Txt_Y+9, SW_EWLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    drawtext(58,  Switch_Txt_Y+9, SW_NSPED_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);       // Show switch and sensor states at bottom of the screen
    drawtext(87,  Switch_Txt_Y+9, SW_NSLT_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    drawtext(112,  Switch_Txt_Y+9, SW_MODE_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
     
    drawtext(100, 10, FlashingS_Txt, ST7735_WHITE, ST7735_BLACK, TS_1); 
    drawtext(70, 10, FlashingR_Txt, ST7735_WHITE, ST7735_BLACK, TS_1);
    
    Do_Init();
    
     
        if (NSPED_SW == 1)
       {
           
           printf ("INT0 interrupt pin detected \r\n");
       }

        if (EWPED_SW == 1)
       {
         
           printf ("INT1 interrupt pin detected \r\n");
       }

        if (FLASHING_REQUEST == 1)
       {
           printf ("INT2 interrupt pin detected \r\n");
       }
    
}





void Do_Init()                     // Initialize the ports

{
    Init_ADC();                         // Initialize ADC converter
    Init_TRIS();                        // Initialize TRIS
    init_UART ();                       // Initiate UART
    OSCCON = 0x70;   
    
    INTCONbits.INT0IF = 0;         // Clear INT0IF
    INTCON3bits.INT1IF = 0;        // Clear INT1IF
    INTCON3bits.INT2IF =0;         // Clear INT2IF
    
    INTCON2bits.INTEDG0 = 0 ;      // INT0 EDGE falling
    INTCON2bits.INTEDG1 = 0;       // INT1 EDGE falling
    INTCON2bits.INTEDG2 = 1;       // INT2 EDGE rising
    
    INTCONbits.INT0IE = 1;         // Set INT0 IE
    INTCON3bits.INT1IE = 1;        // Set INT1 IE
    INTCON3bits.INT2IE = 1;        // Set INT2 IE
    
    INTCONbits.GIE = 1;            // Set the Global Interrupt Enable
} 

void interrupt high_priority chkisr()
{

        if (INTCONbits.INT0IF == 1) INT0_ISR();  // check if INT0 has occurred
        if (INTCON3bits.INT1IF == 1) INT1_ISR();
        if (INTCON3bits.INT2IF == 1) INT2_ISR();
}

void INT0_ISR()
{
    INTCONbits.INT0IF=0;    // Clear the interrupt flag
        if (MODE == 1)
        {    
            NSPED_SW = 1;          // set software INT0_flag
        }
}

void INT1_ISR()
{
    INTCON3bits.INT1IF=0;   // Clear the interrupt flag
        if (MODE == 1)
        {
            EWPED_SW = 1;          // set software INT1_flag
        }
}

void INT2_ISR()
{
    INTCON3bits.INT2IF=0;           // Clear the interrupt flag
    FLASHING_REQUEST = 1;          // set software INT2_flag
}


void Do_Flashing()
{
    FLASHING = 1;
    FLASHING_REQUEST = 0;
    
    while(FLASHING == 1)
    {
       if (FLASHING_REQUEST == 1)  
        {
           FLASHING_REQUEST = 0;
           FLASHING = 0;
        }
       
    NS_RED = 1;
    NSLT_RED = 1;
    EW_RED = 1;
    EWLT_RED = 1;
    WAIT_1_SEC();
           
    NS_RED = 0;
    NSLT_RED = 0;
    EW_RED = 0;
    EWLT_RED = 0;
    WAIT_1_SEC();
    }
}
