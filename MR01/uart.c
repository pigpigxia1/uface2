#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include  <pthread.h>
#include <sys/select.h>
#include <time.h>
#include "uart.h"

const char *Serial_Dev = "/dev/ttyS1";

static int S_fd;
static fd_set readfs;


int one_pack_flag = 0;
int one_pack_num = 0;
int pack_num = 0;
int rev_num = 2;


static unsigned char bcc_check(unsigned char *string,int len)
{
	char bccsum = 0;
	int i;
	for(i = 0; i < len; i++)
	{
		bccsum ^= string[i];
	}
	return bccsum;
	
}


int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
{
    struct termios newtio,oldtio;
    if(tcgetattr(fd,&oldtio)!=0)
    {
        perror("error:SetupSerial 3\n");
        return -1;
    }
    bzero(&newtio,sizeof(newtio));
    //使能串口接收
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    newtio.c_lflag &=~ICANON;//原始模式

    //newtio.c_lflag |=ICANON; //标准模式

    //设置串口数据位
    switch(nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
            break;
        case 8:
            newtio.c_cflag |=CS8;
            break;
    }
    //设置奇偶校验位
    switch(nEvent)

    {
        case 'O':
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'E':
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'N':
            newtio.c_cflag &=~PARENB;
            break;
    }
    //设置串口波特率
    switch(nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio,B2400);
            cfsetospeed(&newtio,B2400);
            break;
        case 4800:
            cfsetispeed(&newtio,B4800);
            cfsetospeed(&newtio,B4800);
            break;
        case 9600:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
        case 460800:
            cfsetispeed(&newtio,B460800);
            cfsetospeed(&newtio,B460800);
            break;
        default:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;
    }
    //设置停止位
    if(nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if(nStop == 2)
        newtio.c_cflag |= CSTOPB;
    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH);

    if(tcsetattr(fd,TCSANOW,&newtio)!=0)
    {
        perror("com set error\n");
        return -1;
    }
    return 0;
}


void * Pthread_Serial( void *arg )
{
    int n=0;
    int ret;
    struct termios oldstdio;
    unsigned char Rx_Data[100];
    unsigned char Tx_Data[50]={0xF2,0x00,0x00,0x03,0x43,0x30,0x33,0x03,0x00};
    
    S_fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( -1==S_fd ) 
        pthread_exit(NULL);
    
    ret = set_opt(S_fd,115200,8,'N',1);
    if(ret == -1)
    {
         pthread_exit(NULL);
    }
	
	//Tx_Data[8] = bcc_check(Tx_Data,8);
	
	//serial_sendx(S_fd, Tx_Data, 9);
	//usleep(100000);
	
	
    printf("start\n");
    while(1)
    {   
		usleep(100000);
        ret = read( S_fd, Rx_Data, 100);
        /*if( ret >0 )
        {
			//printf("read data ret=%d\n",ret);
            Serial_D.len = ret;
            memset( Serial_D.Data, 0, Serial_D.len+3 );
            memcpy( Serial_D.Data, Rx_Data, Serial_D.len );        
            printf("\nread ret = %d %s ",ret,Serial_D.Data);
        }
        else
        {
            
            sprintf( Tx_Data,"send %d\r\n", n++ );
            serial_send( S_fd, Tx_Data );   
			usleep(100000);			
            //printf("send ok%d\r\n",n++);    
        }*/
		if( ret >0 )
        {
			printf("\nread data ret=%d\n",ret);
            //Serial_D.len = ret;
            //memset( Serial_D.Data, 0, Serial_D.len+3 );
            ///memcpy( Serial_D.Data, Rx_Data, Serial_D.len ); 
			for(n = 0; n < ret; n++)
			{
				printf("data[%d]=%x\t",n,Rx_Data[n]);
			}
			fflush(stdout);
				
        }
        else
        {
            //serial_sendx(S_fd, Tx_Data, 7);
            //sprintf( Tx_Data,"send %d\r\n", n++ );
            //serial_send( S_fd, Tx_Data );   
			//usleep(1000000);			
            //printf("send ok%d\r\n",n++);    
        }
    }
    pthread_exit(NULL);
}

int serial_send(char *Data )
{
    int string_num;
    string_num = strlen(Data);
    return  write(S_fd,Data, string_num );
}

int serial_sendx(unsigned char data[], int num)
{
	int send_num = num;
	unsigned char *pdat = data;
	int ret;
	
	do
	{
		ret = write( S_fd,pdat, send_num);
		send_num -= ret;
		pdat += ret;
		
	}while(send_num > 0);
	return num ;
	
}

int serial_read(unsigned char data[],int num)
{
	int read_num = num;
	unsigned char *pdat = data;
	int ret;
	
	do
	{
		ret = select(S_fd+1, &readfs, NULL, NULL, NULL);
		if(ret == 0)
		{
			printf("time out!\n");
			continue;
		}
		else if(ret < 0)
		{
			printf("select error!\n");
			
		}
		else
		{
			if (FD_ISSET(S_fd,&readfs))
			{
				
				ret = read( S_fd, pdat, read_num);
				read_num -= ret;
				pdat += ret;
				//printf("serial_read ret=%d\n",ret);
			}
		}
	}while(read_num > 0);
	//printf("read_num=%d\n",num);
	
	return num;
}



int seial_init(int nSpeed,int nBits,char nEvent,int nStop)
{
	int ret;
	S_fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( -1==S_fd ) 
	{
		printf("open device error!\n");
	}
        
    
    ret = set_opt(S_fd,nSpeed,nBits,nEvent,nStop);
    if(ret == -1)
    {
         printf("set_opt error!\n");
    }
	
	FD_ZERO(&readfs);
	FD_SET(S_fd, &readfs);
	
	return ret;
}

void uart_test()
{
	pthread_t pthread_id;
	
    
    //Create a thread
    pthread_create( &pthread_id, NULL, &Pthread_Serial, NULL );
    
	/*while(1)
	{
		usleep(1000);
	}*/
}

/*
int main()
{
#if 1
    pthread_t pthread_id;
	int cmd;
	int send_len;
	unsigned char Tx_Data[50]={0xF2,0x00,0x00,0x03,0x43,0x30,0x33,0x03,0x00};
    
    //Create a thread
    pthread_create( &pthread_id, NULL, &Pthread_Serial, NULL );
    usleep(1000);
	
	while(1)
	{
		printf("enter cmd!\n");
		scanf("%d",&cmd);
		
		switch(cmd)
		{
			case 1:         //reset
				Tx_Data[2] = 0;
				Tx_Data[3] = 0x03;
				Tx_Data[4] = 0x43; 
				Tx_Data[5] = 0x30;
				Tx_Data[6] = 0x33;
				Tx_Data[7] = 0x03;
				break;
			case 2:           //check
				Tx_Data[3] = 0x03;
				Tx_Data[4] = 0x43; 
				Tx_Data[5] = 0x31;
				Tx_Data[6] = 0x31;
				Tx_Data[7] = 0x03;
				break;
			case 3:
				break;
			case 4:
				break;
			default:
				break;
			
		}
		send_len = Tx_Data[2] * 256 + Tx_Data[3] + 6;
		printf("cmd=%d send_len=%d\n",cmd,send_len);
		Tx_Data[send_len-1] = bcc_check(Tx_Data,send_len-1);
		serial_sendx(Tx_Data, send_len);
		usleep(10000);
	}
	
#else
	
	int n=0;
    int ret;
	
	fd_set readfs;
	struct timeval Timeout;
	time_t now;
	
    unsigned char Rx_Data[30];
	unsigned char buff[30];
	unsigned char *pbuf = Rx_Data;
    unsigned char Tx_Data[20]={0x80,0x05,0x90,0xB6,0x01,0x00,0x00};
    
    S_fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( -1==S_fd ) 
	{
		printf("open device error!\n");
	}
        
    
    ret = set_opt(S_fd,115200,8,'N',1);
    if(ret == -1)
    {
         printf("set_opt error!\n");
    }
	
	Timeout.tv_usec = 0;
	Timeout.tv_sec  = 5;
	
	FD_ZERO(&readfs);
	FD_SET(S_fd, &readfs);
	
	printf("stop RF\n");
	Tx_Data[4] = 0x03;  //stop 
	serial_sendx(S_fd, Tx_Data, 7);
	usleep(100000);
	
	printf("start RF\n");
	Tx_Data[4] = 0x01;  //start
	serial_sendx(S_fd, Tx_Data, 7);
	
    printf("start\n");
    
    while(1)
    {
		//printf("select\n");
		ret = select(S_fd+1, &readfs, NULL, NULL, NULL);
		if(ret == 0)
		{
			printf("time out!\n");
			continue;
		}
		else if(ret < 0)
		{
			printf("select error!\n");
			
		}
		else
		{
			if (FD_ISSET(S_fd,&readfs))
			{
				
				ret = read( S_fd, buff, rev_num);
				printf("\nread data ret=%d\n",ret);
				
			#if 1
				if(one_pack_flag == 0)
				{
					printf("one pack start\n");
					one_pack_flag = 1;
					memcpy( pbuf, buff,ret);
					if((ret >= 2)&&((buff[0] == 0xB0)|| (buff[0] == 0x90)))
					{
						rev_num = buff[1] + 2;
						one_pack_num = rev_num;
					}
					
					
						
				}
				else
				{
					printf("read pack\n");
					
					memcpy( pbuf, buff,ret);
				}
				pbuf += ret;
				rev_num -= ret;
				if(rev_num == 0)
				{
					time(&now);
					one_pack_flag = 0;
					rev_num = 2;
					pbuf = Rx_Data;
					pack_num ++;
					printf("\nread one pack end!send data pack_num = %d time=%d\n",pack_num,now);
					
					for(n = 0; n < one_pack_num; n++)
					{
						printf("%x\t",Rx_Data[n]);
					}
					if(Rx_Data[0] == 0xB0)
					{
						printf("stop RF\n");
						Tx_Data[4] = 0x03;  //stop 
						serial_sendx(S_fd, Tx_Data, 7);
					
						sleep(2);
						printf("start RF\n");
						Tx_Data[4] = 0x01;  //start
						serial_sendx(S_fd, Tx_Data, 7);
						
						
					}
				}
			#else
					
				if( ret >1 )
				{
					printf("\nread data ret=%d\n",ret);
					
					for(n = 0; n < ret; n++)
					{
						printf("%x\t",buff[n]);
					}
						
				}
				
			#endif
			}
		}
        usleep(10000);
        
    }
#endif
    return 0;
}
*/