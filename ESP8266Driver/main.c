#include <msp430.h> 
#include "FR2355_UART_Driver.h"

char text[] = "& W";

char UART_Received_Message [50] = "";
int messageCharCounter = 0;
char UART_Receive_Message_Completed = 0;

enum messageType {
    espInitMessage = 0,
    espStatusMessage = 1,
    espMQTTMessage = 2
};


struct esp_message_contents{
    char msgType;
    char status;
    char *topic;
    char *message;
};


void Init_GPIO();
struct esp_message_contents espMsgHandler(char *espMsg);


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    //PM5CTL0 &= ~LOCKLPM5;

    Init_GPIO();
    uart_Init_9600();

    __bis_SR_register(GIE);

    while(1){
        uart_Print(text);
        while(!UART_Receive_Message_Completed);
        UART_Receive_Message_Completed = 0;
        struct esp_message_contents esp_message = espMsgHandler(UART_Received_Message);
        uart_Print(esp_message.topic);
        uart_Print(esp_message.message);
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


struct esp_message_contents espMsgHandler(char *espMsg){
    struct esp_message_contents message;
    char payload[50] = "";
    char topic[50] = "";
    char currentChar = *espMsg++;
    if ( currentChar == '0'){
        message.msgType = espInitMessage;
        message.status = 1;
    }
    else if (currentChar == '1'){
        message.msgType = espInitMessage;
        message.status = 0;
    }
    else if ( currentChar == '@'){
        message.msgType = espStatusMessage;
        *espMsg++;
        currentChar = *espMsg++;
        if (currentChar == '0'){
            message.status = 0;
        }
        else
        {
            message.status = 1;
        }
    }
    else if (currentChar == '~')
    {
        message.status = 0;
        message.msgType = espMQTTMessage;
        *espMsg++;
        currentChar = *espMsg++;

        int i = 0;
        while (currentChar != ' ')
        {
            topic[i] = currentChar;
            currentChar = *espMsg++;
            i++;
        }

        currentChar = *espMsg++;
        i = 0;
        while (currentChar != 0 )
        {
            payload[i] = currentChar;
            currentChar = *espMsg++;
            i++;
        }
    }

    message.topic = topic;
    message.message = payload;
    return message;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    char receivedChar;
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      receivedChar = UCA1RXBUF;
      if ((receivedChar == '\n') | (receivedChar == 0x0d))
      {
          UART_Received_Message[messageCharCounter] = 0;
          UART_Receive_Message_Completed = 1;
      }
      else
      {
          UART_Received_Message[messageCharCounter] = receivedChar;
          messageCharCounter++;
      }


        //while(!(UCA1IFG&UCTXIFG));
      //UCA1TXBUF = UCA1RXBUF;
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    default: break;
  }
}
