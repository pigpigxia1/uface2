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

const unsigned char Tx_opendev[7]={0x80,0x05,0x90,0xB6,0x01,0x00,0x00};
const unsigned char Tx_closedev[7]={0x80,0x05,0x90,0xB6,0x03,0x00,0x00};

#define OK 0
#define ERR -1
#define ID_LEN  8

typedef enum{
	READ_OK = 2,         //读数据正常
	OPENED = 1,          //设备打开
	CLOSED = 0,           //设备关闭
	//OK = 0,
	OPEN_ERR  = -1,        //设备打开错误
	CLOSE_ERR = -2,        
	SEND_ERR	= -3,      //
	READ_ERR	= -4,
	UART_ERR	= -5       //串口设置错误
	
	
}DEVICE_STATUS;

typedef struct{
	DEVICE_STATUS mdev_status;    //状态位
	struct timeval mTimeout;      //超时时间
	unsigned char *id_buf;
}DEV_INFO, *PDEV_INFO;

const char *Serial_Dev = "/dev/ttyS1";


static int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop)
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
        return ERR;
    }
    return OK;
}

static int serial_sendx(int fd,unsigned char data[], int num)
{
	int send_num = num;
	unsigned char *pdat = data;
	int ret;
	
	do
	{
		ret = write( fd,pdat, send_num);
		send_num -= ret;
		pdat += ret;
		
	}while(send_num > 0);
	return OK ;
	
}

static int serial_read(int fd,unsigned char data[],int num,struct timeval *Timeout)
{
	int read_num = num;
	unsigned char *pdat = data;
	int ret;
	fd_set readfs;
	struct timeval timeout;
	
	timeout.tv_usec = 0;
	timeout.tv_sec  = 5;
	FD_ZERO(&readfs);
	FD_SET(fd, &readfs);
	
	do
	{
		ret = select(fd+1, &readfs, NULL, NULL, &timeout);
		if(ret == 0)
		{
			printf("time out!\n");
			//continue;
			break;
		}
		else if(ret < 0)
		{
			printf("select error!\n");
			
		}
		else
		{
			if (FD_ISSET(fd,&readfs))
			{
				
				ret = read( fd, pdat, read_num);
				read_num -= ret;
				pdat += ret;
				//printf("serial_read ret=%d\n",ret);
			}
		}
	}while((read_num > 0)/* && (ret > 0)*/);
	//printf("read_num=%d\n",num);
	
	return num;
}
/*
static int serial_init(int nSpeed,int nBits,char nEvent,int nStop)
{
	int ret;
	S_fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( -1==S_fd ) 
	{
		printf("open device error!\n");
	}
	ret = set_opt(S_fd,115200,8,'N',1);
	
	FD_ZERO(&readfs);
	FD_SET(S_fd, &readfs);
	
	return ret;
}*/

static int read_pack(int fd,unsigned char *id_buff,struct timeval *Timeout)
{
	int ret;
	//int one_pack_flag = 0;
	int one_pack_num = 0;
	int rev_num = 2;
	
	unsigned char buff[20];
	unsigned char *pbuf = id_buff;
	
	ret = serial_read(fd,buff, rev_num,Timeout);
	//printf("\nread data ret=%d\n",ret);
	
	if((ret >= 2)&&((buff[0] == 0xB0)|| (buff[0] == 0x90)))
	{
		memcpy( pbuf, buff,ret);
		rev_num = buff[1];
		//one_pack_num = rev_num + 2;
	}
	else
	{
		printf("\nread pack failed!\n");
		return ERR;
	}
		
	pbuf += ret;
	//rev_num -= ret;
	ret = serial_read(fd,buff, rev_num,Timeout);
	memcpy( pbuf, buff,ret);
	pbuf = NULL;
	return one_pack_num;
}

int get_id(int fd,PDEV_INFO dev_info)
{
	unsigned char buff[10];
	
	if(dev_info->mdev_status == CLOSED /*|| dev_info->mdev_status != OPENED */)
	{
		printf("please start device!\n");
		return ERR;
	}
	/*dev_info->mdev_status = read_pack(dev_info->mfd,buff);
	if(dev_info->mdev_status != READ_OK)
	{
		return ERR;
	}*/
	if(read_pack(fd,buff,&dev_info->mTimeout) < 0)
	{
		dev_info->mdev_status = READ_ERR;
		tcflush(fd,TCIOFLUSH);
		return READ_ERR;
	}
	memcpy(dev_info->id_buf, buff+2,ID_LEN);
	tcflush(fd,TCIOFLUSH);
	
	dev_info->mdev_status = READ_OK;
	return OK;
}
int device_open(int nSpeed,int nBits,char nEvent,int nStop,PDEV_INFO dev_info)
{
	int ret;
	int fd;
	unsigned char rebuf[4];
	
	
	if(dev_info->mdev_status == OPENED)
	{
		printf("device is running!\n");
		return OPENED;
	}
	
	dev_info->mdev_status = OPEN_ERR;
	fd = open( Serial_Dev, O_RDWR | O_NOCTTY |O_NDELAY);
    if( fd < 0 ) 
	{
		dev_info->mdev_status = UART_ERR;
		printf("open uart error!\n");
		return UART_ERR;
	}
	ret = set_opt(fd,nSpeed,nBits,nEvent,nStop);  //设置串口
	if(ret < 0)
	{
		dev_info->mdev_status = UART_ERR;
		goto uart_err;
		//return UART_ERR;
	}
	
	//FD_ZERO(&readfs);
	//FD_SET(fd, &readfs);
	
	//serial_init(int fd,int nSpeed,int nBits,char nEvent,int nStop);
	serial_sendx(fd,Tx_opendev,7);      //打开设备
	
	read_pack(fd,rebuf,&dev_info->mTimeout);
	
	if(rebuf[2] == 0x90 && rebuf[3] == 0x0)
	{
		dev_info->mdev_status = OPENED;
		return fd;
	}
	
	serial_sendx(fd,Tx_closedev, 7);
	
uart_err:
	close(fd);

	return ERR;
	
	
}

int close_device(int fd,PDEV_INFO dev_info)
{
	int ret = -1;
	unsigned char rebuf[4];
	
	
	if(dev_info->mdev_status == CLOSED)
	{
		printf("device have closed!\n");
		return ERR;
	}
	tcflush(fd,TCIOFLUSH);
	serial_sendx(fd,Tx_closedev, 7);
	read_pack(fd,rebuf,&dev_info->mTimeout);
	
	if(rebuf[2] == 0x90 && rebuf[3] == 0x0)
	{
		close(fd);
		
		dev_info->mdev_status = CLOSED;
		return OK;
	}
	
	close(fd);
	dev_info->mdev_status = CLOSE_ERR;
	return ERR;
}


int main()
{
	int cmd;
	int i;
	int fd;
	unsigned char id[8];
	
	DEV_INFO dev_info;
//	dev_info.mTimeout.tv_usec = 0;
//	dev_info.mTimeout.tv_sec = 10;
	dev_info.id_buf = id;
	while(1)
	{
		printf("enter cmd 1:start 2: get_id 3: stop\n");
		scanf("%d",&cmd);
		switch(cmd)
		{
			case 1:
			if((fd = device_open(115200,8,'N',1,&dev_info)) < 0)
			{
				printf("open device error\n");
			}
			else
			{
				printf("open device sucsss\n");
			}
				
			break;
			
			case 2:
			if(get_id(fd,&dev_info) < 0)
			{
				printf("get id error!\n");
			}
			else
			{
				printf("get id sucess\nid=");
				for(i = 0;i < 8; i++)
					printf("%x\t",id[i]);
			}
			break;
			
			case 3:
			if(close_device(fd,&dev_info) < 0)
			{
				printf("close device error\n");
			}
			else
			{
				printf("close device sucsss\n");
			}
			break;
			case 4:
			//close_device(fd,&dev_info);
			
			if((fd = device_open(115200,8,'N',1,&dev_info)) < 0)
			{
				printf("open device error\n");
			}
			else
			{
				printf("open device sucsss\n");
			}
			
			if(get_id(fd,&dev_info) < 0)
			{
				printf("get id error!\n");
			}
			else
			{
				printf("get id sucess\nid=");
				for(i = 0;i < 8; i++)
					printf("%x\t",id[i]);
			}
			close_device(fd,&dev_info);
			
			break;
		}
		//printf("dev_status=%d\n",dev_info.mdev_status);
	}
}