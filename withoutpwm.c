

#define _XTAL_FREQ 20000000 // 20 MHz osilatör frekans?

#define RS RD2
#define EN RD3
#define D4 RD4
#define Buzzer RB4
#define D5 RD5
#define D6 RD6
#define D7 RD7
#define Trigger RB1 //34 is Trigger
#define Echo RB2//35 is Echo 

#include <xc.h>

#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)

void Lcd_SetBit(char data_bit) {
    if(data_bit & 1) 
        D4 = 1;
    else
        D4 = 0;

    if(data_bit & 2)
        D5 = 1;
    else
        D5 = 0;

    if(data_bit & 4)
        D6 = 1;
    else
        D6 = 0;

    if(data_bit & 8) 
        D7 = 1;
    else
        D7 = 0;
}

void Lcd_Cmd(char a) {
    RS = 0;           
    Lcd_SetBit(a);
    EN = 1;         
    __delay_ms(4);
    EN = 0;         
}

void Lcd_Clear() {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b) {
    char temp, z, y;
    if(a == 1) {
        temp = 0x80 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    } else if(a == 2) {
        temp = 0xC0 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        Lcd_Cmd(z);
        Lcd_Cmd(y);
    }
}

void Lcd_Start() {
    Lcd_SetBit(0x00);
    for(int i = 1065244; i <= 0; i--)  NOP();
    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03); 
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Print_Char(char data) {
    char Lower_Nibble, Upper_Nibble;
    Lower_Nibble = data & 0x0F;
    Upper_Nibble = data & 0xF0;
    RS = 1;
    Lcd_SetBit(Upper_Nibble >> 4);
    EN = 1;
    for(int i = 2130483; i <= 0; i--)  NOP();
    EN = 0;
    Lcd_SetBit(Lower_Nibble);
    EN = 1;
    for(int i = 2130483; i <= 0; i--)  NOP();
    EN = 0;
}

void Lcd_Print_String(char *a) {
    int i;
    for(i = 0; a[i] != '\0'; i++)
        Lcd_Print_Char(a[i]);
}

int time_taken;
int distance;
char t1, t2, t3, t4, t5;
char d1, d2, d3;

void delay_ms(unsigned int milliseconds) {
    while(milliseconds > 0) {
        __delay_ms(1);
        milliseconds--;
    }
}

int main() {
    TRISD = 0x00; // PORTD declared as output for interfacing LCD
    TRISB0 = 1; // Define the RB0 pin as input to use as interrupt pin
    TRISB1 = 0; // Trigger pin of US sensor is set as output pin
    TRISB2 = 1; // Echo pin of US sensor is set as input pin
    TRISB3 = 0; // RB3 is output pin for LED
    TRISB4 = 0; // for buzzer

    T1CON = 0x20;
    const float calibration_factor = 1.5;

    Lcd_Start();

    Lcd_Set_Cursor(1, 1);
    Lcd_Print_String("Ultrasonic sensor");
    Lcd_Set_Cursor(2, 1);
    Lcd_Print_String("with PIC16F877A");

    __delay_ms(2000);
    Lcd_Clear();

    while(1) {
        TMR1H = 0;
        TMR1L = 0;

        Trigger = 1;
        __delay_us(10);
        Trigger = 0;

        while (Echo == 0);
        TMR1ON = 1;
        while (Echo == 1);
        TMR1ON = 0;

        time_taken = (TMR1L | (TMR1H << 8));
        distance = (0.05 * time_taken) / 2;
        time_taken = time_taken * 0.8;

        t1 = (time_taken / 1000) % 10;
        t2 = (time_taken / 1000) % 10;
        t3 = (time_taken / 100) % 10;
        t4 = (time_taken / 10) % 10;
        t5 = (time_taken / 1) % 10;

        d1 = (distance / 100) % 10;
        d2 = (distance / 10) % 10;
        d3 = (distance / 1) % 10;

        Lcd_Set_Cursor(1, 1);
        Lcd_Print_String("Time_taken:");
        Lcd_Print_Char(t1 + '0');
        Lcd_Print_Char(t2 + '0');
        Lcd_Print_Char(t3 + '0');
        Lcd_Print_Char(t4 + '0');
        Lcd_Print_Char(t5 + '0');

        Lcd_Set_Cursor(2, 1);
        Lcd_Print_String("distance:");
        Lcd_Print_Char(d1 + '0');
        Lcd_Print_Char(d2 + '0');
        Lcd_Print_Char(d3 + '0');

        if(distance <= 4) {
            Buzzer = 1;  // Buzzer sürekli çalar
        } else if(distance <= 12) {
            int on_time = 150 - ((12 - distance) * 10); // 150ms ile 50ms aras?nda de?i?ir
            int off_time = 75 - ((12 - distance) * 5); // 75ms ile 25ms aras?nda de?i?ir

            Buzzer = 1;
            delay_ms(on_time);
            Buzzer = 0;
            delay_ms(off_time);
        } else {
            Buzzer = 0;
        }
    }

    return 0;
}
