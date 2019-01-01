#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>
#include  <pthread.h>
#include <time.h>
#include "uart.h"
#include "CRC.h"
#include "bt.h"


char *help = "\n1:check status\n \
2:disconnect\n \
3:connect\n \
enter cmd:";

int main(int argc,char *argv[])
{
	int cmd;
	int mode;
	int fd;
	int cmd_len = 0;
	unsigned char cmd_buf[20] = {0x55,0x55};
	
	fd = uart_init("/dev/ttyS1",19200,8,'N',1);
	if(fd < 0)
	{
		printf("uart open error\n");
		return -1;
	}
	
	printf("enter mode 1:open_door 2:step by step");
	scanf("%d",&mode);
	while(1)
	{
		//printf(help);
		scanf("%d",&cmd);
		if(mode == 1)
		{
			
			open_door(fd,NULL,0);
		}
		else if(mode == 2)
		{
			bt_ctr(fd,cmd);
		
		/*	 switch(cmd)
			{
				case 1:
					cmd_buf[2] = 0x07;
					cmd_buf[3] = 0x03;
					cmd_buf[4] = 0xB4;
					cmd_buf[5] = 0xEE;
					cmd_buf[6] = 0xEE;
					cmd_len = 7;
					break;
				case 2:
					cmd_buf[2] = 0x07;
					cmd_buf[3] = 0x02;
					cmd_buf[4] = 0xB3;
					cmd_buf[5] = 0xEE;
					cmd_buf[6] = 0xEE;
					cmd_len = 7;
					break;
				case 3:
					cmd_buf[2] = 0x0D;
					cmd_buf[3] = 0x11;
					printf("enter mac:\n");
					scanf("%2x %2x %2x %2x %2x %2x",&cmd_buf[4],&cmd_buf[5],&cmd_buf[6],&cmd_buf[7],&cmd_buf[8],&cmd_buf[9]);
					printf("mac:%2x %2x %2x %2x %2x %2x",cmd_buf[4],cmd_buf[5],cmd_buf[6],cmd_buf[7],cmd_buf[8],cmd_buf[9]);
					cmd_buf[10] = calculate_verify(cmd_buf,10);
					printf("lrc=%x",cmd_buf[10]);
					cmd_buf[11] = 0xEE;
					cmd_buf[12] = 0xEE;
					cmd_len = 13;
					break;
				default:
					break;
				
			}
			
			serial_sendx(fd,cmd_buf,cmd_len); */
		}
	}
	
	return 0;
}
