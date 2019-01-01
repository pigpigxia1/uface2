#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include  <pthread.h>

const char *Serial_Dev = "/dev/ttyS1";

typedef struct {
    char R_flag;
    char W_flag;
    int  len;
    char Data[255];
}Serial;

typedef struct {
    int Forward;
    int left;
    int rotate;
    unsigned char Check;
    char Enter[3];
}Vehicle;


Vehicle Serial_Tx = {0,0,0,0,{"\r\n"}};
Serial Serial_D = {0,0,0,{0}};
int S_fd;


int wait_flag = 0;

int serial_send( int fd, char *Data );
int set_opt(int fd,int nSpeed,int nBits,char nEvent,int nStop);

void * Pthread_Serial( void *arg )
{
    int n=0;
    int ret;
    struct termios oldstdio;
    char Rx_Data[100];
    char Tx_Data[50]={0};
    
    S_fd = open( Serial_Dev, O_RDWR|O_NOCTTY );
    if( -1==S_fd ) 
        pthread_exit(NULL);
    
    ret = set_opt(S_fd,115200,8,'N',1);
    if(ret == -1)
    {
         pthread_exit(NULL);
    }
    printf("start\n");
    while(1)
    {   
		
        ret = read( S_fd, Rx_Data, 100);
        if( ret >0 )
        {
            Serial_D.len = ret;
            memset( Serial_D.Data, 0, Serial_D.len+3 );
            memcpy( Serial_D.Data, Rx_Data, Serial_D.len );        
            printf("%s",Serial_D.Data);
        }
        else
        {
            usleep(100000);
            sprintf( Tx_Data,"send %d\r\n", n++ );
            serial_send( S_fd, Tx_Data );    
            //printf("send ok%d\r\n",n++);    
        }
    }
    pthread_exit(NULL);
}

int main()
{
    pthread_t pthread_id;
    
    //Create a thread
    pthread_create( &pthread_id, NULL, &Pthread_Serial, NULL );
    usleep(1000);

    if( -1==S_fd )
    {
        printf("error: cannot open serial dev\r\n");
        return -1;
    }   
    
    while(1)
    {
        usleep(1000);
        
    }
    
    return 0;
}

int serial_send( int fd, char *Data )
{
    int string_num;
    string_num = strlen(Data);
    return  write( S_fd,Data, string_num );
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