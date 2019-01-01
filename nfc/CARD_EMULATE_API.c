#include "FM175XX_REG.h"
#include "FM175XX.h"
#include "CARD_EMULATE_API.h"

//#include "READER_API.h"
//#include "UART.h"

 struct CARD_STRUCT CARD;
							
void Card_Emulate_Init(int fd)
{
	 SetReg(fd,JREG_TXMODE,JBIT_TXCRCEN);// Activate CRC
   SetReg(fd,JREG_RXMODE,JBIT_RXCRCEN);// Activate CRC
   SetReg(fd,JREG_RXTRESHOLD,0x84);	// MinLevel/ColLevel
	 SetReg(fd,JREG_DEMOD,0x61);		// DemodReg
	 
	 SetReg(fd,JREG_MIFARE,BIT5|BIT3|BIT1);		// MifareReg
	 SetReg(fd,JREG_GSNLOADMOD,0x08);//调节卡下拉深度：bit7~bit4:未调制时的负载；bit3~bit0:调制时的负载
	 SetReg(fd,JREG_TXBITPHASE,0x87);//使用场恢复时钟,发送相位 = 7
	 SetReg(fd,JREG_RFCFG,0x67);		//Bit6~Bit4:RxGain ,Bit3~Bit0:RFLevel
   SetReg(fd,JREG_WATERLEVEL,0x20);	//设置FIFOWater Level
   SetReg(fd,JREG_COMMIEN,JBIT_IRQINV|JBIT_RXI|JBIT_ERRI);	//中断为低电平触发		
   SetReg(fd,JREG_DIVIEN,JBIT_IRQPUSHPULL|JBIT_RFONI|JBIT_RFOFFI);
 	 ModifyReg(fd,JREG_CONTROL,JBIT_INITIATOR,RESET);		//清除Initiator位，进入卡模式
   return ;
}

unsigned char Card_Emulate_Config(int fd,unsigned char *configbuf)
{
	unsigned char reg_data;
	
	Clear_FIFO(fd);
  SPI_Write_FIFO(fd,25,configbuf);//SENS_RES(2 bytes),NFCID1(3 bytes),SEL_RES(1 byte),NFCID3(1 byte)

	SetReg(fd,JREG_COMMAND,CMD_CONFIGURE);
	usleep(1000);			//1ms
	GetReg(fd,JREG_COMMAND,&reg_data);	
	if((reg_data & CMD_MASK) != CMD_IDLE)	
		return ERROR;
	return SUCCESS;
}

void Card_Emulate_AutoColl(int fd)
{
	SetReg(fd,JREG_COMMAND,CMD_IDLE);
	//Uart_Send_Msg("AutoColl\r\n");
	SetReg(fd,JREG_COMMAND,CMD_AUTOCOLL);		
	return ;
}

unsigned char Card_Emulate_IRQ_Event(int fd,unsigned char tag_com_irq,unsigned char tag_div_irq)
{
	unsigned char reg_com_irq,reg_div_irq;
		CARD.IRQ = FALSE;	
		GetReg(fd,JREG_COMMIRQ,&reg_com_irq);		
		GetReg(fd,JREG_DIVIRQ,&reg_div_irq);		
		
		reg_com_irq &= tag_com_irq;
		reg_div_irq	&= tag_div_irq;			

						
		if(reg_div_irq & JBIT_RFONI)
			{			
				FM175XX_SoftReset(fd);
			  Card_Emulate_Init(fd);
				Card_Emulate_Config(fd,CARD_UID_INFO);			
				Card_Emulate_AutoColl(fd);
				printf(">RF_ON\r\n");
				SetReg(fd,JREG_DIVIRQ,JBIT_RFONI);//清除RF进场中断	
			}			
			
		if(reg_div_irq & JBIT_RFOFFI)
			{
				printf(">RF_OFF\r\n");
				SetReg(fd,JREG_COMMAND,CMD_IDLE);
				SetReg(fd,JREG_DIVIRQ,JBIT_RFOFFI);//清除RF离场中断
//				FM175XX_SoftReset();
//			  Card_Emulate_Init();
//				Card_Emulate_Config(CARD_UID_INFO);
//				Uart_Send_Msg("RF_ON\r\n");
//				Card_Emulate_AutoColl();			
			}
		
		if(reg_com_irq & JBIT_RXI)	
			{
				CARD.RecvFlag = TRUE;	//收到数据,接收标=TRUE					
			}
			
		if(reg_com_irq & JBIT_ERRI)	
			{
				Clear_FIFO(fd);
				SetReg(fd,JREG_COMMAND,CMD_RECEIVE);	 //进入接收状态				
				SetReg(fd,JREG_COMMIRQ,JBIT_ERRI);				
			} 		
	return SUCCESS;
}

/*********************************************************************************************************
** 函数名称:	Card_Emulate_RX
** 函数功能:	
** 输入参数:		rbuf:读取数据
** 输出参数:		无
** 返回值:			读取的数据长度
*********************************************************************************************************/
void Card_Emulate_RX(int fd,unsigned char *rece_buf,unsigned char *rece_len)
{
	unsigned char regdata;
	unsigned char length;

	*rece_len = 0;
	length = 0;	
		
	do
	{	
		if(FM175XX_Get_FIFOlength(fd) >= 32)	//查FIFO是否到32字节
		{
			SPI_Read_FIFO(fd,32,&rece_buf[*rece_len]);		//渐满之后读32字节
					
			*rece_len += 32;
		}		
		GetReg(fd,JREG_COMMIRQ,&regdata);	
	}
	while(!(regdata & JBIT_RXI));

	length = FM175XX_Get_FIFOlength(fd);	//

	SPI_Read_FIFO(fd,length,&rece_buf[*rece_len]);		//读最后的数据
	
	*rece_len += length;			
	
	return;
}

/*********************************************************************************************************
** 函数名称:	FM175XX_Card_TX
** 函数功能:	RF数据回发
** 输入参数:		ilen:回发数据长度
** 					*ibuf:回发的数据
** 输出参数:		无
** 返回值:			是否读取成功
*********************************************************************************************************/
unsigned char Card_Emulate_TX(int fd,unsigned char ilen,unsigned char *ibuf)
{
	unsigned char regdata;
	unsigned char slen;
	unsigned char *sbuf;
	
	/*Uart_Send_Msg("<-Send = ");	Uart_Send_Hex(ibuf,ilen);	Uart_Send_Msg("\r\n");*/
	
	slen = ilen;
	sbuf = ibuf;

	Clear_FIFO(fd);
	if(slen <= 64)
	{
		SPI_Write_FIFO(fd,slen,sbuf);		//write fifo	长度小于64字节，全部写入FIFO
		slen = 0;
	}
	else
	{
		SPI_Write_FIFO(fd,64,sbuf);			//write fifo	长度大于64字节，先写入64字节
		
		slen -= 64;		//待发长度－64
		sbuf += 64;		//待发数据指针+64
	}
		
	SetReg(fd,JREG_COMMIRQ,JBIT_TXI);		//清除TXI中断
	SetReg(fd,JREG_COMMAND,CMD_TRANSMIT);	   			//启动发送		

	while(slen>0)
	{		
		if(FM175XX_Get_FIFOlength(fd)<=32)
		{
			if(slen<=32)
			{
				SPI_Write_FIFO(fd,slen,sbuf);			//FIFO写入剩余字节
				slen = 0;				
			}
			else
			{
				SPI_Write_FIFO(fd,32,sbuf);			//FIFO写入32字节
				
				slen -= 32; 	//待发长度-32
				sbuf += 32; 	//待发数据指针+32
			}
		}
	}

	do
	{
		GetReg(fd,JREG_COMMIRQ,&regdata);		
	}
	while( !(regdata&JBIT_TXI));	//等待发送结束		
	
	return SUCCESS;
}

