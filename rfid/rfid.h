#ifdef __RFID_H
#define __RFID_H


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


int get_id(int fd,PDEV_INFO dev_info);
int device_open(int nSpeed,int nBits,char nEvent,int nStop,PDEV_INFO dev_info);
int close_device(int fd,PDEV_INFO dev_info);

#endif