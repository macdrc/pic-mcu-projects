
#define _XTAL_FREQ      8000000

extern char diff; 
#define ACK             1
#define NAK             0

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02


#define FAN_LED         PORTDbits.RD7                                           //Set Fan Led
#define RTC_ALARM_NOT   PORTBbits.RB2                                           //Set RTC_Alarm on DS323 pin 2
#define FAN_EN          PORTEbits.RE0                                           //Set Fan Enable Pin
#define FAN_PWM         PORTCbits.RC2                                           //Set Fan PWM Pin

#define Ch_Minus        0
#define Channel         1
#define Ch_Plus         2
#define Prev            3
#define Next            4
#define Play_Pause      5
#define Minus           6
#define Plus            7
#define EQ              8

#define OFF 0x00  
#define RED 0x01                                                                                                    
#define GRN 0x02
#define YLW 0x03
#define BLU 0x04
#define PRP 0x05
#define CYA 0x06
#define WHT 0x07

