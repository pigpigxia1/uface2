#include <stdint.h>
#include <string.h> 
#include "FM175XX_REG.h"
#include "FM175XX.h"
#include "gpio.h"
#include "uart.h"

#include <unistd.h>


#define REST_PIN   229



static void REST_PIN_init()
{
	gpio_export(REST_PIN);
	gpio_setDir(REST_PIN,1);
	gpio_setValue(REST_PIN,1);
}


static void uart_senddat(int fd,unsigned char regdata)
{
	write( fd,&regdata, 1);
}

static unsigned char uart_read(int fd)
{
	unsigned char dat;
	int ret = -1;
	
	fd_set readfs;
	struct timeval Timeout;
	
	Timeout.tv_usec = 0;
	Timeout.tv_sec  = 2;
	FD_ZERO(&readfs);
	FD_SET(fd, &readfs);
	
	ret = select(fd+1, &readfs, NULL, NULL, &Timeout);
	if(ret == 0)
	{
		printf("time out!\n");
			
	}
	else if(ret < 0)
	{
		printf("select error!\n");
		
	}
	else
	{
		if (FD_ISSET(fd,&readfs))
		{
			
			read( fd, &dat, 1);
			
		}
	}
	
	//serial_read(fd,&dat,1);
	
	return dat;
}


void pcd_Init(void)
{
    //SPI_Init();
    //CD_CfgTPD();                                                        /* 配置复位管脚                 */
	
	REST_PIN_init();
	//fd = uart_init(9600,8,'N',1);
	//return fd;
}

void pcd_DeInit()
{
	gpio_unexport(REST_PIN);
	//close(fd);
}


//***********************************************
//函数名称：Read_Reg(uchar addr,uchar *regdata)
//函数功能：读取寄存器值
//入口参数：addr:目标寄存器地址   regdata:读取的值
//出口参数：uchar  TRUE：读取成功   FALSE:失败
//***********************************************
void GetReg(int fd,uchar Reg,uchar *regdata)
{	
		unsigned char addr;
	
	addr = Reg | 0x80;
	uart_senddat(fd,addr);
	
	*regdata = uart_read(fd);
	return ;
}

//***********************************************
//函数名称：Write_Reg(uchar addr,uchar* regdata)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   regdata:要写入的值
//出口参数：uchar  TRUE：写成功   FALSE:写失败
//***********************************************
void SetReg(int fd,uchar Reg,uchar regdata)
{	
			unsigned char addr;
	
	addr = Reg & 0x7F;	
	uart_senddat(fd,addr);
	uart_read(fd);
	uart_senddat(fd,regdata);
	
	return ;
}

//*******************************************************
//函数名称：ModifyReg(uchar addr,uchar* mask,uchar set)
//函数功能：写寄存器
//入口参数：addr:目标寄存器地址   mask:要改变的位  
//         set:  0:标志的位清零   其它:标志的位置起
//出口参数：
//********************************************************
void ModifyReg(int fd,uchar addr,uchar mask,uchar set)
{
	uchar regdata;
	
	GetReg(fd,addr,&regdata);
	
		if(set)
		{
			regdata |= mask;
		}
		else
		{
			regdata &= ~mask;
		}

	SetReg(fd,addr,regdata);
	return ;
}

void SPI_Write_FIFO(int fd ,u8 reglen,u8* regbuf)  //SPI接口连续写FIFO寄存器 
{
	u8 i;
    if(reglen==0)
        return;
    for(i=0;i<reglen;i++) {
        SetReg(fd,JREG_FIFODATA, *(regbuf+i));
    }

	return;
}

void SPI_Read_FIFO(int fd,u8 reglen,u8* regbuf)  //SPI接口连续读FIFO
{
	u8 i;
    if (reglen==0)
		return;
    
    for(i=0;i<reglen;i++) {
       GetReg(fd,JREG_FIFODATA,regbuf+i);
    }

	return;
}


void Clear_FIFO(int fd)
{
	u8 regdata;
		
	GetReg(fd,JREG_FIFOLEVEL,&regdata);
	if((regdata & 0x7F) != 0)			//FIFO如果不空，则FLUSH FIFO
	{
	SetReg(fd,JREG_FIFOLEVEL,JBIT_FLUSHFIFO);
	}
	return ;
}

void FM175XX_HardReset(void)
{	
	gpio_setValue(REST_PIN,0);
	usleep(10000);
	gpio_setValue(REST_PIN,1);
	usleep(10000);
	return;
}



//***********************************************
//函数名称：GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//函数功能：读取扩展寄存器值
//入口参数：ExtRegAddr:扩展寄存器地址   ExtRegData:读取的值
//出口参数：unsigned char  TRUE：读取成功   FALSE:失败
//***********************************************
void GetReg_Ext(int fd,unsigned char ExtRegAddr,unsigned char* ExtRegData)
{

	SetReg(fd,JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_RD_ADDR + ExtRegAddr);
	GetReg(fd,JREG_EXT_REG_ENTRANCE,&(*ExtRegData));
	return;	
}

//***********************************************
//函数名称：SetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData)
//函数功能：写扩展寄存器
//入口参数：ExtRegAddr:扩展寄存器地址   ExtRegData:要写入的值
//出口参数：unsigned char  TRUE：写成功   FALSE:写失败
//***********************************************
void SetReg_Ext(int fd,unsigned char ExtRegAddr,unsigned char ExtRegData)
{

	SetReg(fd,JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_ADDR + ExtRegAddr);
	SetReg(fd,JREG_EXT_REG_ENTRANCE,JBIT_EXT_REG_WR_DATA + ExtRegData);
	return; 	
}

//*******************************************************
//函数名称：ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char* mask,unsigned char set)
//函数功能：寄存器位操作
//入口参数：ExtRegAddr:目标寄存器地址   mask:要改变的位  
//         set:  0:标志的位清零   其它:标志的位置起
//出口参数：unsigned char  TRUE：写成功   FALSE:写失败
//********************************************************
void ModifyReg_Ext(int fd,unsigned char ExtRegAddr,unsigned char mask,unsigned char set)
{
  unsigned char regdata;
	
	GetReg_Ext(fd,ExtRegAddr,&regdata);
	
		if(set)
		{
			regdata |= mask;
		}
		else
		{
			regdata &= ~(mask);
		}
	
	SetReg_Ext(fd,ExtRegAddr,regdata);
	return;
}

unsigned char FM175XX_SoftReset(int fd)
{	
	unsigned char reg_data;
	SetReg(fd,JREG_COMMAND,CMD_SOFT_RESET);
	usleep(1000);//FM175XX芯片复位需要1ms
	GetReg(fd,JREG_COMMAND,&reg_data);
	if(reg_data == 0x20)
		return SUCCESS;
	else
		return ERROR;
}


/*********************************************************************************************************
** 函数名称:	FM175XX_Get_FIFOlength
** 函数功能:	读取FIFO长度
** 输入参数:	无
** 输出参数:	无
** 返回值: 		FIFO长度
*********************************************************************************************************/
extern unsigned char FM175XX_Get_FIFOlength(int fd)
{
	unsigned char regdata;
	
	GetReg(fd,JREG_FIFOLEVEL,&regdata);

	regdata &= 0x7F;

	return regdata;
}


unsigned char Set_EMV_Enhance(int fd,unsigned char set)
{
  unsigned char reg_data;
	GetReg(fd,JREG_VERSION,&reg_data);
	
    if(reg_data == 0x8A)//V09
        {
					//Uart_Send_Msg("IC = V09\r\n");
					GetReg_Ext(fd,JREG_DSP_VER,&reg_data);
					//Uart_Send_Msg("DSP = ");Uart_Send_Hex(&reg_data,1);Uart_Send_Msg("\r\n");
					
          ModifyReg(fd,JREG_RFU1E,BIT5,set);//V09	打开DSP功能
					SetReg_Ext(fd,JREG_DSP_CONTROL,BIT5|BIT4|BIT3|BIT2|BIT0);//DSP RESET
        }
    
  return SUCCESS;
}





