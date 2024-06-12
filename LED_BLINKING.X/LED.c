// PIC16F877A LED blink example
// http://simple-circuit.com/
 
#include <xc.h>
#define _XTAL_FREQ 4000000 //define crystal frequency to 20MHz

void main(void) 
{
TRISB0=0; //set RC0 pin as a digital output pin
while(1)
{
   RB0 = 1;                  // set RC0 pin to logic High &  turn on 
   __delay_ms(1000);  //add delay of 1 second 
   RB0 = 0;                  // set RC0 pin to logic low & turn off
  __delay_ms(1000);  //add delay of 1 second

}
return;
}

