#ifndef __UART_H
#define __UART_H

uint8_t calculate_verify(uint8_t * data,uint8_t length);
//unsigned int crc_check(uint8_t* data, unsigned int length);
int uart_init(char *dev,int nSpeed,int nBits,char nEvent,int nStop);
int serial_sendx(int fd, unsigned char data[], int num);
int serial_send( int fd, char *Data );




#endif