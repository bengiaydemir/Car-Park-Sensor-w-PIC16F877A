/*ultrasonic sensor code*/



#define _XTAL_FREQ 8000000

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>

#pragma config FOSC = HS   
#pragma config WDTE = OFF  
#pragma config PWRTE = OFF 
#pragma config BOREN = ON 
#pragma config LVP = OFF   
#pragma config CPD = OFF   
#pragma config WRT = OFF  
#pragma config CP = OFF    


void main()
{ 
  int a;

  TRISB = 0b00010000;         //RB4 as Input PIN (ECHO)
  TRISD = 0x00;               // LCD Pins as Output

  T1CON = 0x10;   
  
   while(1)
  { 
      TMR1H = 0;                //Sets the Initial Value of Timer
      TMR1L = 0;                //Sets the Initial Value of Timer

      RB0 = 1;                  //TRIGGER HIGH
      __delay_us(10);           //10uS Delay 
      RB0 = 0;                  //TRIGGER LOW

      while(!RB4);              //Waiting for Echo
      TMR1ON = 1;               //Timer Starts
      while(RB4);               //Waiting for Echo goes LOW
      TMR1ON = 0;               //Timer Stops

      a = (TMR1L | (TMR1H<<8)); //Reads Timer Value
      a = a/58.82;              //Converts Time to Distance
      a = a + 1;                //Distance Calibration

}
}


 


  
