#ifndef __UART_H
#define __UART_H

unsigned char calculate_verify(unsigned char * data,unsigned char length);
int calculate_verify16(unsigned char * data,int length);
unsigned int crc_check(unsigned char* data, unsigned int length);
int uart_init(char *dev,int nSpeed,int nBits,char nEvent,int nStop);
int serial_sendx(int fd, unsigned char data[], int num);
int serial_send( int fd, char *Data );




#endif