#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include <p18f4620.h>
#include "utils.h"

extern char found;
extern char Nec_code1;
extern short nec_ok;
extern char array1[21];
extern char duty_cycle;

char check_for_button_input()
{       
    if (nec_ok == 1)
    {
            nec_ok = 0;


            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge

            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_code1 == array1[j]) 
                {
                    found = j;
                    j = 21;
                }
            }
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
                return (0);
            }
            else
            {
                return (1);
            }
    }
}

char bcd_2_dec (char bcd)
{
    int dec;
    dec = ((bcd>> 4) * 10) + (bcd & 0x0f);
    return dec;
}

int dec_2_bcd (char dec)
{
    int bcd;
    bcd = ((dec / 10) << 4) + (dec % 10);
    return bcd;
}

void Do_Beep()
{
    KEY_PRESSED = 1;
    Activate_Buzzer();
    Wait_One_Sec();
    KEY_PRESSED = 0;
    Deactivate_Buzzer();
    do_update_pwm(duty_cycle);
}

void Do_Beep_Good()
{
    Activate_Buzzer_2KHz();
}

void Do_Beep_Bad()
{
Activate_Buzzer_500Hz();

}

void Wait_One_Sec()
{
    for (int k=0;k<0x8000;k++);
}

void Activate_Buzzer()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_500Hz()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000111 ;
    CCPR1L = 0b01111100 ;
    CCP1CON = 0b00111100 ;
}

void Activate_Buzzer_2KHz()
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR1L = 0b01111100 ;
CCP1CON = 0b00111100 ;
}

void Activate_Buzzer_4KHz()
{
PR2 = 0b01111100 ;
T2CON = 0b00000101 ;
CCPR1L = 0b00111110 ;
CCP1CON = 0b00011100 ;
}

void Deactivate_Buzzer()
{
    CCP2CON = 0x0;
	PORTBbits.RB3 = 0;
}

void do_update_pwm(char duty_cycle) 
{ 
	float dc_f;
	int dc_I;
	PR2 = 0b00000100 ;                      // Set the frequency for 25 Khz 
	T2CON = 0b00000111 ;                    // As given in website
	dc_f = ( 4.0 * duty_cycle / 20.0) ;     // calculate factor of duty cycle versus a 25 Khz signal
	dc_I = (int) dc_f;                      // Truncate integer
	if (dc_I > duty_cycle) dc_I++;          // Round up function
	CCP1CON = ((dc_I & 0x03) << 4) | 0b00001100;
	CCPR1L = (dc_I) >> 2;
}

void Set_RGB_Color(char color)
{
    // add code here
}

float read_volt()
{
     int nStep = get_full_ADC ();    // Get ADC steps
     volt = nStep*5/1024.0;   // Multiply by reference V = 5v
}

unsigned int get_full_ADC()
{
    int result;
    ADCON0bits.GO=1;                    // Start Conversion
    while(ADCON0bits.DONE==1);          // wait for conversion to be completed
    result = (ADRESH * 0x100) + ADRESL; // combine result of upper byte and
                                        // lower byte into result
    return result;                      // return the result.
}

void Init_ADC()
{
       ADCON0 = 0b00000001;                // 00 0000 0 1 Select Channel AN0 (bit 5-2), A/D idle (bit 1) A/D enable (bit 0)
    ADCON1 = 0b00001110;                // Bit0 - analog Bit5 - AVss 000\11\011                            
    ADCON2 = 0xA9;   
}


