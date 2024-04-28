#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#include <p18f4620.h>
#include "utils.h"
#include "Main.h"

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

            INTCONbits.INT0IE = 1;                                              // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;                                            // Edge programming for INT0 falling edge

            found = 0xff;                                                       //set found to FF
            for (int j=0; j< 21; j++)                                           //for statment to go thorugh all 21 values
            {
                if (Nec_code1 == array1[j])                                     //set nec code to arry1 with value j
                {
                    found = j;                                                  //set found to j
                    j = 21;                                                     //set j to 21
                }
            }
            
            if (found == 0xff)                                                  //if found is value FF
            {
                printf ("Cannot find button \r\n");                             //print invalid button
                return (0);                                                     //renturn a 0
            }
            else                                                                //else
            {
                return (1);                                                     //return a 1
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
   
    Activate_Buzzer();                                                          //Actives the buzzer
    Wait_One_Sec();                                                             //waits on sec
    
    Deactivate_Buzzer();                                                        //deactive the buzzer
    do_update_pwm(duty_cycle);                                                  //updates the pwm with the value of duty cycle
}

void Do_Beep_Good()
{
// add code here using Activate_Buzzer_2KHz()
    
    Activate_Buzzer_2KHz();                                                     //calls activate the 2khz buzzer func
    Wait_One_Sec();                                                             //waits one sec
  
    Deactivate_Buzzer();                                                        //deactivates the buzzer
    do_update_pwm(duty_cycle);                                                  //updates the pwm with the value duty cucle
}

void Do_Beep_Bad()
{
// add code here using Activate_Buzzer_500Hz()
    
    Activate_Buzzer_500Hz();                                                    //calls activate 500hz buzzer func 
    Wait_One_Sec();                                                             //waits one sec  
    
    Deactivate_Buzzer();                                                        //deactivate the buzzer
    do_update_pwm(duty_cycle);                                                  //updates the pwm with the value of duty cycle
}

void Wait_One_Sec()
{
    for (int k=0;k<0x8000;k++);                                                 //one sec equivalint 
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
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_2KHz()
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01111100 ;
    CCP2CON = 0b00111100 ;
}

void Activate_Buzzer_4KHz() 
{
    PR2 = 0b01111100 ;
    T2CON = 0b00000101 ;    
    CCPR2L = 0b00111110 ;
    CCP2CON = 0b00011100 ;
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

float read_volt()
{
    float num_step = get_full_ADC(); 
    float voltage_v = num_step*4/1024.0;
    
    //0.00488;
    return voltage_v; 
    
   
}

unsigned int get_full_ADC(void){ // get result from light resistor 
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

void Init_ADC(){ // INITIALIZE ADC CONVERT
    ADCON0 = 0b00000111; //0x01;
    ADCON1 = 0x0D;
    ADCON2 = 0xA9;
}

void Set_RGB_Color(char color)
{
    unsigned char masked=0x8F;
    unsigned char masked_d=PORTD & masked;
    int D3[8] = {OFF,RED,GRN,YLW,BLU,PRP,CYA,WHT};
    PORTD = (D3[color]<<4)|masked_d;
    Wait_One_Sec();
}
