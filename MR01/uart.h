#ifndef __UAER_H
#define __UAER_H


int seial_init(int nSpeed,int nBits,char nEvent,int nStop);
int serial_read(unsigned char data[],int num);
int serial_sendx(unsigned char data[], int num);
int serial_send(char *Data );
void uart_test();

#endif