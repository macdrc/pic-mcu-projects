#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char FAN;
extern char duty_cycle;

#define D1R 0b00000001
#define D1G 0b00000010
#define D1Y 0b00000011
#define D1B 0b00000100
#define D1P 0b00000101
#define D1C 0b00000110
#define D1W 0b00000111

#define D2R 0b00100000
#define D2G 0b01000000
#define D2Y 0b01100000
#define D2B 0b10000000
#define D2P 0b10100000
#define D2C 0b11000000
#define D2W 0b11100000



char D1[8] = {0, D1R, D1G, D1Y, D1B, D1P, D1C, D1W};
char D2[7] = { D2R, D2G, D2Y, D2B, D2P, D2C, D2W};



#define FANEN_LED     PORTEbits.RE2

//char D2[9] = {D2R, D2Y, D2G, D2B, D2M, D2C, D2W, D2W, D2W};

extern char found;

int get_RPM()
{
    int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev,then RPS = count /2
    TMR3L = 0; // clear out the count
    return (RPS * 60); // return RPM = 60 * RPS 
}

void Toggle_Fan()
{
    if (FAN == 0)
        Turn_On_Fan();
    else
        Turn_Off_Fan();
}

void Turn_Off_Fan()
{
    FAN = 0;
    FAN_EN = 0;
    FANEN_LED = 1;
}

void Turn_On_Fan()
{
    FAN = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FANEN_LED = 0;
}

void Increase_Speed()
{
    if (duty_cycle == 100)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(100);
    }
    else
    {
        duty_cycle += 5;
        do_update_pwm(duty_cycle);
    }
    
    
}

void Decrease_Speed()
{
 if (duty_cycle == 0)
    {
        Do_Beep();
        Do_Beep();
        do_update_pwm(0);
    }
    else
    {
        duty_cycle -= 5;
        do_update_pwm(duty_cycle);
    }
}

void do_update_pwm(char duty_cycle) 
{ 
    float dc_f;
    int dc_I;
    PR2 = 0b00000100 ; // set the frequency for 25 Khz
    T2CON = 0b00000111 ; //
    dc_f = ( 4.0 * duty_cycle / 20.0) ; // calculate factor of duty cycle versus a 25 Khz signal
    dc_I = (int) dc_f; // get the integer part
    if (dc_I > duty_cycle) dc_I++; // round up function
    CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
    CCPR1L = (dc_I) >> 2;
}

void Set_DC_RGB(int duty_cycle)
{   
    char index;
    index = duty_cycle /10;
    if (duty_cycle >= 70)
    {
        PORTD = (PORTD & 0b11111000);
        PORTD = PORTD | 0b00000111;
    }
    else
    {
        PORTD = (PORTD & 0b11111000);
        PORTD = PORTD | index;
    }
//    char color = 0x00;
//    if(duty_cycle >= 70){
//        color = D1W;
//    }                                         // if greater than 70 stay white
//    else{
//        color = D1[duty_cycle /10] & 0b00000111;
//    }
    
}

void Set_RPM_RGB(int rpm)
{
    unsigned valuemasked;
    valuemasked = (PORTD & 0b00011111);
    int RPM = (rpm/500);
    if (rpm<1){
        PORTD = valuemasked;
    }
    else{
        PORTD = (valuemasked | (D2[RPM]) | (D1[found])) ;
    }

}

void Do_Beep()
{
    Activate_Buzzer();
    Wait_One_Sec();
    Deactivate_Buzzer();
    do_update_pwm(duty_cycle);
    
}

void Wait_One_Sec()
{
    for (int k=0;k<17000;k++);
}

void Activate_Buzzer()
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0; 
}

