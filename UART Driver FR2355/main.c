#include <msp430.h> 
#include "FR2355_UART_Driver.h"

char text[] = "I am an MSP430\r\n";

void Init_GPIO();

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    //PM5CTL0 &= ~LOCKLPM5;

    Init_GPIO();
    uart_Init_9600();

    while(1){
        uart_Print(text);
    }


    return 0;
}


void Init_GPIO(){
    P4DIR = 0xFF; P2DIR = 0xFF;
    P4REN = 0xFF; P2REN = 0xFF;
    P4OUT = 0x00; P2OUT = 0x00;

    // Configure UART pins
    P4SEL0 |= BIT2 | BIT3;                    // set 2-UART pin as second function
}
