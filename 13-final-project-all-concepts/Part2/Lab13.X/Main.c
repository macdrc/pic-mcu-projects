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
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"
#include "utils.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config CCP2MX = PORTBE


//Define all varibles used

void Test_Alarm();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char heater_set_temp = 75;
unsigned char Nec_state = 0;
float volt;
char INT1_flag, INT2_flag;

short nec_ok = 0;
char Nec_code1;
char HEATER;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;
int alarm_mode, MATCHED,color;

// TEXT Storage

char buffer[31]     = "ECE3301L F'22 S1 \0";
char *nbr;                                                                      //general buffer
char *txt;                                                                      //buffer for tex
char tempC[]        = "+25";                                                    //text storage for temp c
char tempF[]        = "+77";                                                    //text storage for temp F
char time[]         = "00:00:00";                                               //text storage for time
char date[]         = "00/00/00";                                               //text storage for date
char alarm_time[]   = "00:00:00";                                               //text storage for alarm time
char Alarm_SW_Txt[] = "OFF";                                                    //text storage for Alarm Sw
char Heater_Set_Temp_Txt[] = "075F";                                            //text storage for heater set temp
char Heater_SW_Txt[]   = "OFF";                                                 // text storage for Heater Mode
char DC_Txt[]       = "000";                                                    // text storage for Duty Cycle value
char Volt_Txt[]     = "0.00V";                                                  //text storage for Volt
char RTC_ALARM_Txt[]= "0";                                                      //text storage for RTC alarm
char RPM_Txt[]      = "0000";                                                   // text storage for RPM
char setup_time[]       = "00:00:00";                                           // text storage for setup time
char setup_date[]       = "01/01/00";                                           // text storage for setup date
char setup_alarm_time[] = "00:00:00";                                           // text storage for alarm time
char setup_heater_set_text[]   = "075F";                                        // text storage for setup heater


//Button array values 
char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52};


void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()                                                                //Keep tera term at 19200 speed
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}



void Do_Init()                                                                  // Initialize the ports 
{ 
    init_UART();                                                                // Initialize the uart
    Init_ADC();                                                                 // Intizalize ADC converter
    OSCCON=0x70;                                                                // Set oscillator to 8 MHz 
    
    ADCON1=0x0D;                                                                //Using bit 1 for analog signal
    TRISA = 0x03;                                                               //Sets bit 1 to input
    TRISB = 0x27;                                                               //Sets bits 0,1,2,5 to inputs 
    TRISC = 0x01;                                                               //Sets bit 0 to input
    TRISD = 0x00;                                                               //Sets portd d to outputs
    TRISE = 0x00;                                                               //sets porte to outputs
    PORTE = 0x00;

    HEATER = 0;                                                                 //sets heater to off
    RBPU=0;                                                                     //set rbpu to off

    TMR3L = 0x00;                                                               //sets timer 3 to off
    T3CON = 0x03;                                                               //enables bit 0 and 
    I2C_Init(100000); 

    DS1621_Init();                                                              //initalize the temp sensor
    Init_Interrupt();                                                           //intialize the intrrupt call
    Turn_Off_Fan();                                                             //calls turn off fan func
    heater_set_temp = 99;                                                       //sets heater temp to 99
}


void main() 
{
    Do_Init();                                                                  // Initialize do init
    Initialize_Screen();                                                        //initizlize screen
    DS3231_Turn_Off_Alarm();                                                    //call turn off alarm
    DS3231_Read_Alarm_Time();                                                   // Read alarm time for the first time

    tempSecond = 0xff;                                                          //set temp second to ff
    while (1)                                                                   //do forever
    {
        DS3231_Read_Time();                                                     //read current time

        if(tempSecond != second)                                                //if temp sec does not equal current second
        {
            tempSecond = second;                                                //set temp sec to currentsec
            rpm = get_RPM();                                                    //set rpm to called get rpm value
            volt = read_volt();                                                 //set volt to called read volt
            DS1621_tempC = DS1621_Read_Temp();                                  //set temp to called read temp
            if ((DS1621_tempC & 0x80) == 0x80) 
                DS1621_tempC = - (0x80 - DS1621_tempC & 0x7f);
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;  
            //conver C to F 
             //printf ("\\\\VOlts are %f\\\n\n", volt);
                     
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);//print the time
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);        //print the tempratures
            printf ("alarm = %d Heater = %d ", RTC_ALARM_NOT, HEATER);          //print the status of alarm and heater
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);                  //print rpm and duty cycle
            //printf ("PORTD = %d",PORTD);
            Set_DC_RGB(duty_cycle);                                             //call dc rbg with duty_cycle value
            Set_RPM_RGB(rpm);                                                   //call rpm rbg with rpm value
            Monitor_Heater();                                                   //call monitor heater
            Test_Alarm();                                                       //call test alarm
            Update_Screen();                                                    //call update screen
        }
        
        if (check_for_button_input() == 1)                                      //check for a button pressed
        {

            nec_ok = 0;                                                         //set flag to 0
            switch (found)                                                      //switch statment for found
            {
                case Ch_Minus:                                                  //case for ch minus button
                    Do_Beep_Good();                                             //call good beep
                    Do_Setup_Time();                                            //call setup time
                    break;                                                      //breaks 
                
                case Channel:                                                   //case for channel button
                    Do_Beep_Good();                                             //call good beep
                    Do_Setup_Alarm_Time();                                      //call setup alarm time
                    break;                                                      //break
                    
                case Ch_Plus:                                                   //case for ch plus button
                    Do_Beep_Good();                                             //call for good beep
                    Do_Setup_Heater_Temp();                                     //call for setup heater
                    break;                                                      //breaks
                    
                case Play_Pause:                                                //case for play/pause button
                    Do_Beep_Good();                                             //call for goo beep
                    Toggle_Heater();                                            //call for toggle heater
                     break;                                                     //breaks
        
                default:                                                        //if none of above buttons pushed do this
                    Do_Beep_Bad();                                              //call for bad beep
                    break;                                                      //breaks
            }
        }    

        // add code depending on the connection of your push-button switch to handle either INT1_flag
        // INT2_flag 
        if (INT1_flag == 1)                                                     //if interrupt 1 is flagged
        {
           ALARMEN = !ALARMEN;                                                  //set alarm enable to not value of alarm enable
            INT1_flag = 0;                                                      //set flag back to 0
        }
        
        if (INT2_flag == 1)                                                     //if interrput 2 is flagged
        {
            //ALARMEN = !ALARMEN; 
            INT2_flag = 0;                                                      //set flag back to 0
        }
        
    }
}

//void Test_Alarm()
//{
//    if(!alarm_mode && ALARMEN){                                                 //if not alarm mode and'd with alarmen is true 
//        DS3231_Turn_On_Alarm();                                                 //call turn on alarm
//        alarm_mode = 1;                                                         //set alarm mode to 1
//    }
//    else if(alarm_mode && !ALARMEN){                                           //else if alarm mode and'd with not alarmen
//        DS3231_Turn_Off_Alarm();                                                //call turn off alarm
//        alarm_mode = 0;                                                         //set alarm mode to off
//        Set_RGB_Color(OFF);                                                     //set alarm rbg to off
//        Deactivate_Buzzer();  
//        ALARMEN = 0;//call deactive the buzzer
//    }
//    else if (alarm_mode == 1 && ALARMEN ==1)                                   //else if alarm mode is 1 and'd with alarm en =1  
//    {
//            
//        if(RTC_ALARM_NOT == 0)                                                  //if rtc alrm is 0
//        {
//            MATCHED = 1;                                                        //set matched to 1
//            Activate_Buzzer();                                                  //call activate buzzer
//            do_update_pwm(duty_cycle);
//        }
//        else 
//            
//        if (MATCHED == 1)                                                       //if matched is 1
//        {
//            Activate_Buzzer();
//            Set_RGB_Color(color++);                                             //call alarm rbg with value of color incrementing 
//            if(color>7)                                                         //if color is greater than 7
//                color =0; 
//  
//                
//            if (volt > 2.5)                                                     //if voltage is greater than 3
//            {
//                MATCHED = 0;                                                    //set matched to 0
//                color =0;                                                       //set color to 0
//                Set_RGB_Color(OFF);                                           //call alarm rgb with value of color
//                Deactivate_Buzzer();                                            //call deactivae buzzer
//                do_update_pwm(duty_cycle);                                      //call update pwm with value of duty cycle
//                alarm_mode = 0;                                                 //set alarm mode to 0
//                ALARMEN=0; 
//                
//                //set alarm enable to 0
//            }
//
//        }    
//    }
//    else if (alarm_mode == 1 && ALARMEN == 0)
//    {
//        MATCHED = 0;                                                    //set matched to 0
//                color =0;                                                       //set color to 0
//                Set_RGB_Color(OFF);                                           //call alarm rgb with value of color
//                Deactivate_Buzzer();                                            //call deactivae buzzer
//                do_update_pwm(duty_cycle);                                      //call update pwm with value of duty cycle
//                alarm_mode = 0;                                                 //set alarm mode to 0
//                ALARMEN=0; 
//    }
//}


void Test_Alarm()
{
    if(!alarm_mode && ALARMEN){                                                 //if not alarm mode and'd with alarmen is true 
        DS3231_Turn_On_Alarm();                                                 //call turn on alarm
        alarm_mode = 1;                                                         //set alarm mode to 1
    }
    else if(alarm_mode && !ALARMEN){                                           //else if alarm mode and'd with not alarmen
        DS3231_Turn_Off_Alarm();                                                //call turn off alarm
        alarm_mode = 0;                                                         //set alarm mode to off
        Set_RGB_Color(OFF);                                                     //set alarm rbg to off
        Deactivate_Buzzer();  
        MATCHED = 0;//call deactive the buzzer
    }else if (alarm_mode == 1 && ALARMEN ==1)                                   //else if alarm mode is 1 and'd with alarm en =1  
    
    {
     
        if(RTC_ALARM_NOT == 0)                                                  //if rtc alrm is 0
        {
            MATCHED = 1;                                                        //set matched to 1
            Activate_Buzzer();                                                  //call activate buzzer
            do_update_pwm(duty_cycle);
        }
        if (MATCHED == 1)                                                       //if matched is 1
        {   Activate_Buzzer();
            Set_RGB_Color(color++);                                             //call alarm rbg with value of color incrementing 
            if(color>7)                                                         //if color is greater than 7
                color =0;                                                       //set color to 0
            
            if (volt > 3 || ALARMEN == 0)                                                     //if voltage is greater than 3
            {
                MATCHED = 0;                                                    //set matched to 0
                color =0;                                                       //set color to 0
                Set_RGB_Color(OFF);                                           //call alarm rgb with value of color
                Deactivate_Buzzer();                                            //call deactivae buzzer
                do_update_pwm(duty_cycle);                                      //call update pwm with value of duty cycle
                alarm_mode = 0;                                                 //set alarm mode to 0
                //ALARMEN=0;                                                      //set alarm enable to 0
            }
        
        }    
    }   
}




////void Test_Alarm()
////{
////    if (alarm_mode == 0 && ALARMEN == 1)
////    {
////        DS3231_Turn_On_Alarm();
////        alarm_mode = 1;
////    }
////    
////    else if (alarm_mode == 1 && ALARMEN == 0)
////    {
////        DS3231_Turn_Off_Alarm();                                                //call turn off alarm
////        alarm_mode = 0;                                                         //set alarm mode to off
////        Set_RGB_Color(OFF);                                                     //set alarm rbg to off
////        Deactivate_Buzzer();  
////        ALARMEN = 0;//call deactive the buzzer
////    }
////    
////    else if (alarm_mode == 1 && ALARMEN == 1)
////    {
////        if (RTC_ALARM_NOT == 0)
////        {
////            MATCHED = 1;
////            Activate_Buzzer();                                                  //call activate buzzer
////            do_update_pwm(duty_cycle);
////        }
////     
////        
////        if (MATCHED == 1 && volt > 1.0)
////        {
////            MATCHED = 0;
////            Deactivate_Buzzer;
////            PORTA = 0x00;
////            DS3231_Turn_Off_Alarm();
////        }
////        
////        else if (MATCHED == 1)
////        {
////             Activate_Buzzer();
////            Set_RGB_Color(color++);                                             //call alarm rbg with value of color incrementing 
////            if(color>7)                                                         //if color is greater than 7
////                color =0;
////    
////        }
////        
////    }
////}
