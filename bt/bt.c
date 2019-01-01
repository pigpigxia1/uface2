#include "bt.h"
#include "uart.h"
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

unsigned char test_connect[] = {0x55,0x55,0x0D,0x11,0xDD,0x0D,0x68,0xA3,0xE4,0x49,0xEA,0xEE,0xEE};
unsigned char test_connect1[] = {0x55,0x55,0x0D,0x11,0xE9,0x82,0x99,0xD8,0x39,0xF8,0xEA,0xEE,0xEE};
unsigned char test_disconnect[] = {0X55,0X55,0X07,0X02,0XB3,0XEE,0XEE};
unsigned char test_active[]={0x5e,0xdc,0x00,0x28,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x48,0x59,0x4f,0x30,0x30,0x30,0x30,0x30,0x31,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x47,0xae,0x2b};
unsigned char get_time[] = {0x5e,0xdc,0x00,0x10,0x03,0x01,0x01,0x00,0x00,0x03,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xae,0x2b};
unsigned char open_door_buf[] = {0x5e,0xdc,0x00,0x10,0x04,0x01,0x01,0x00,0x00,0x03,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xae,0x2b};

int connect = 0;

int bt_ctr(int fd,int cmd)
{
	static unsigned char bit = 0;
	int crc;
	int cmd_len = 0;
	unsigned char *buf;
	
	switch(cmd)
	{
		case 1:
			buf = test_connect;
			cmd_len = test_connect[2];
			open_door_buf[10] = 0x0c;
			get_time[10] = 0x0c;
			break;
		case 2:
			bit++;
			buf = test_active;
			//buf[6] = bit;
			cmd_len = buf[2]*256 + buf[3] + 6;
			
			break;
		case 3:
			bit++;
			buf = get_time;
			cmd_len = buf[2]*256 + buf[3] + 6;
			buf[6] = bit;
			crc = calculate_verify16(&buf[4],cmd_len -8);
			buf[cmd_len-4] = crc / 256;
			buf[cmd_len-3] = crc % 256;
			break;
		case 4:
			bit++;
			buf = open_door_buf;
			buf[6] = bit;
			cmd_len = buf[2]*256 + buf[3] + 6;
			crc = calculate_verify16(&buf[4],cmd_len -8);
			buf[cmd_len-4] = crc / 256;
			buf[cmd_len-3] = crc % 256;
			break;
		case 5:
			buf = test_connect1;
			cmd_len = test_connect1[2];
			buf[10] = calculate_verify(test_connect1,10);
			open_door_buf[10] = 0x1a;
			get_time[10] = 0x1a;
			break;
		case 6:
			buf = test_disconnect;
			cmd_len = test_disconnect[2];
			break;
		default:
		
			break;
	}
	if(bit == 0x0)
		bit = 0x1;
	write(fd,buf,cmd_len);
	
	return 0;
}

int open_door(int fd,char *str,int len)
{
	//bt_ctr(fd,6);
	connect = 0;
	bt_ctr(fd,1);
	printf("connect dev\n");
	while(connect == 0)
	{
		usleep(10000);
		//printf("connect == 0\n");
	}
	printf("device connect\n");
	usleep(100000);
	bt_ctr(fd,2);
	printf("active\n");
	usleep(100000);
	bt_ctr(fd,4);
	printf("open_door\n");
	sleep(1);
	bt_ctr(fd,6);
	
	return 0;
}