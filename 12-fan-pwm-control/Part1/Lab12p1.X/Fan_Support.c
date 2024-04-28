#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"


extern char FAN;
extern char duty_cycle;


#define D1R 0b00000001
#define D1G 0b00000010
#define D1Y 0b00000110
#define D1B 0b00000010
#define D1P 0b00000011
#define D1C 0b00000110
#define D1W 0b00000111

#define D2R 0b00000001
#define D2G 0b00000010
#define D2Y 0b00000110
#define D2B 0b00000010
#define D2P 0b00000011
#define D2C 0b00000110
#define D2W 0b00000111

#define D3R 0b00000001
#define D3G 0b00000010
#define D3Y 0b00000110
#define D3B 0b00000010
#define D3P 0b00000011
#define D3C 0b00000110
#define D3W 0b00000111

char D1[8] = {0, D1R, D1G, D1Y, D1B, D1P, D1C, D1W};
char D2[8] = {0, D2R, D2G, D2Y, D2B, D2P, D2C, D2W};
char D3[21] = {D3R, D3R, D3R, D3C, D3C, D3G, D3B, D3B, D3P, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W, D3W};






int get_RPM()
{
    int RPS = TMR3L / 2; // read the count. Since there are 2 pulses per rev
    // then RPS = count /2
    TMR3L = 0; // clear out the count
    return (RPS * 60); // return RPM = 60 * RPS 
}

void Toggle_Fan()
{
 
}

void Turn_Off_Fan()
{

}

void Turn_On_Fan()
{
 
}

void Increase_Speed()
{



}

void Decrease_Speed()
{
 
}

void do_update_pwm(char duty_cycle) 
{ 
   float dc_f;
   int dc_I;
   PR2 = 0b00000100 ; // set the frequency for 25 Khz
   T2CON = 0b00000111 ; //
   dc_f = ( 4.0 * duty_cycle / 20.0) ; // calculate factor of duty cycle versus a 25 Khz
   // signal
   dc_I = (int) dc_f; // get the integer part
   if (dc_I > duty_cycle) dc_I++; // round up function
   CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
   CCPR1L = (dc_I) >> 2; 
}

void Set_DC_RGB(int duty_cycle)
{
    PORTD = D1[duty_cycle /10] && 0b00111000;
}

void Set_RPM_RGB(int rpm)
{
    int rpm_new = 
    PORTD = D2[rpm] && 0b11000111;
}

void Do_Beep()
{

}

void Wait_One_Sec()
{

}

void Activate_Buzzer()
{

}

void Deactivate_Buzzer()
{

}

