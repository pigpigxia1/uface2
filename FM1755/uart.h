#ifdef __UART_H
#define __UART_H


void uart_senddat(unsigned char regdata);
unsigned char uart_read();
void uart_init(int nSpeed,int nBits,char nEvent,int nStop);


#endif