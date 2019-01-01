#include "string.h"

#include "FM175XX_REG.h"
#include "READER_API.h"
#include "FM175XX.h"
#include "MIFARE.h"

struct type_a_struct Type_A; 
struct type_b_struct Type_B;

void SetTxCRC(int fd,unsigned char mode)
{
	if(mode)
			ModifyReg(fd,JREG_TXMODE,BIT7,SET);
		else
			ModifyReg(fd,JREG_TXMODE,BIT7,RESET);
return;
}

void SetRxCRC(int fd,unsigned char mode)
{
	if(mode)
			ModifyReg(fd,JREG_RXMODE,BIT7,SET);
		else
			ModifyReg(fd,JREG_RXMODE,BIT7,RESET);
	return;
}


unsigned char SetTimeOut(int fd,unsigned int microseconds)
{
	unsigned long  timereload;
	unsigned int  prescaler;

	prescaler = 0;
	timereload = 0;
	while(prescaler < 0xfff)
	{
		timereload = ((microseconds*(long)13560)-1)/(prescaler * 2 + 1);
		
		if( timereload < 0xffff)
			break;
		prescaler++;
	}
		timereload = timereload & 0xFFFF;
	  ModifyReg(fd,JREG_TMODE,0x0F,RESET);
		ModifyReg(fd,JREG_TMODE,prescaler >> 8,SET);
		SetReg(fd,JREG_TPRESCALER,prescaler & 0xFF);					
		SetReg(fd,JREG_TRELOADHI,timereload >> 8);
		SetReg(fd,JREG_TRELOADLO,timereload & 0xFF);
    return FM175XX_SUCCESS;
}

/******************************************************************************/
//函数名：Command_Execute
//入口参数：
//			unsigned char* sendbuf:发送数据缓冲区	unsigned char sendlen:发送数据长度
//			unsigned char* recvbuf:接收数据缓冲区	unsigned char* recvlen:接收到的数据长度
//出口参数
/******************************************************************************/
unsigned char Command_Execute(int fd,command_struct *comm_status)
{
	unsigned char reg_data,send_length,receive_length,send_finish;
	unsigned char irq;
	unsigned char result;
	send_length = comm_status->nBytesToSend;
	receive_length = 0;
	send_finish = 0;//发送完成标志
	comm_status->nBitsReceived = 0;
	comm_status->nBytesReceived = 0;
	comm_status->CollPos = 0;
	comm_status->Error = 0;
	
	SetReg(fd,JREG_COMMAND,CMD_IDLE);
	Clear_FIFO(fd);	
	SetReg(fd,JREG_WATERLEVEL,0x20);//waterlevel = 32字节
	SetReg(fd,JREG_DIVIEN,0x00);
	SetReg(fd,JREG_COMMIEN,0x80);	
	SetReg(fd,JREG_COMMIRQ,0x7F);	
	SetReg(fd,JREG_TMODE,0x80);			
	
	if((comm_status->Cmd == CMD_TRANSCEIVE)||(comm_status->Cmd == CMD_TRANSMIT))
		{		
		SetReg(fd,JREG_COMMAND,comm_status->Cmd);			
		}
		
	if(comm_status->Cmd == CMD_AUTHENT)
		{	
		SPI_Write_FIFO(fd,send_length,comm_status->pSendBuf);
		send_length = 0;
		SetReg(fd,JREG_BITFRAMING,BIT7 | comm_status->nBitsToSend);
		SetReg(fd,JREG_COMMAND,comm_status->Cmd);	
		}				
		
	while(1)
		{			
				GetReg(fd,JREG_COMMIRQ,&irq);
				if(irq & BIT0)//TimerIRq = 1
				{
					SetReg(fd,JREG_COMMIRQ,BIT0);	
					result = FM175XX_TIMER_ERR;		
					break;
				}
				
				if(irq & BIT1)//ERRIRq = 1
				{			
					GetReg(fd,JREG_REGERROR,&reg_data);	
					comm_status->Error = reg_data;
					
					if(comm_status->Error & BIT3)
					{
						GetReg(fd,JREG_COLL,&reg_data);	
						comm_status->CollPos = reg_data & 0x1F;//读取冲突位置
						result = FM175XX_COLL_ERR;
						break;	
					}
				
					result = FM175XX_COMM_ERR;			
					SetReg(fd,JREG_COMMIRQ,BIT1);			
					break;	
				}
				
				if(irq & BIT2)//LoAlterIRq = 1
				{
					if(send_length > 0)
						{
								
						if(send_length > 32)
						{			
							SPI_Write_FIFO(fd,32,comm_status->pSendBuf);	
							comm_status->pSendBuf = comm_status->pSendBuf + 32;
							send_length = send_length - 32;
						}
						else
						{					
							SPI_Write_FIFO(fd,send_length,comm_status->pSendBuf);							
							send_length = 0;						
						}					
						SetReg(fd,JREG_BITFRAMING,BIT7 | comm_status->nBitsToSend);
						}
					SetReg(fd,JREG_COMMIRQ,BIT2);
				}
				
				if(irq & BIT3)//HiAlterIRq = 1
				{
					if(send_finish == 1)//确认发送完成
					{
					SPI_Read_FIFO(fd,32,comm_status->pReceiveBuf + comm_status->nBytesReceived);
					comm_status->nBytesReceived = comm_status->nBytesReceived + 32;
					}
					SetReg(fd,JREG_COMMIRQ,BIT3);
				}
				
				if((irq & BIT4)&&(comm_status->Cmd == CMD_AUTHENT))//IdleTRq = 1
				{
					SetReg(fd,JREG_COMMIRQ,BIT4);			
					result = FM175XX_SUCCESS;
					break;	
				}				
				
				if((irq & BIT5)&&((comm_status->Cmd == CMD_TRANSCEIVE)||(comm_status->Cmd == CMD_RECEIVE)))//RxIRq = 1
				{		
					if(send_finish == 1)//确认发送完成
						{
						GetReg(fd,JREG_CONTROL, &reg_data);
						comm_status->nBitsReceived = reg_data & 0x07;//接收位长度				
						GetReg(fd,JREG_FIFOLEVEL,&reg_data);
						receive_length = reg_data & 0x7F;//接收字节长度														
						SPI_Read_FIFO(fd,receive_length, comm_status->pReceiveBuf + comm_status->nBytesReceived);
						comm_status->nBytesReceived = comm_status->nBytesReceived + receive_length; 
						if((comm_status->nBytesToReceive != comm_status->nBytesReceived)&&(comm_status->nBytesToReceive != 0))
								{
								result = FM175XX_COMM_ERR;//接收到的数据长度错误
								break;
								}		
						}		
					SetReg(fd,JREG_COMMIRQ,BIT5);			
					result = FM175XX_SUCCESS;					
					break;						
				}
				
				if(irq & BIT6)//TxIRq = 1
				{
					SetReg(fd,JREG_COMMIRQ,BIT6);
					if(comm_status->Cmd == CMD_TRANSMIT)
						{
						result = FM175XX_SUCCESS;
						break;					
						}	
					if(comm_status->Cmd == CMD_TRANSCEIVE)
						send_finish = 1;//发送完成
				}
				
		}
	
	ModifyReg(fd,JREG_BITFRAMING,BIT7,RESET);//CLR Start Send	
	SetReg(fd,JREG_COMMAND,CMD_IDLE);
	return result;
}

void FM175XX_Initial_ReaderA(int fd)
{
	
	SetReg(fd,JREG_TXMODE,0x00);
	SetReg(fd,JREG_RXMODE,0x00);	
	SetReg(fd,JREG_MODWIDTH,MODWIDTH);				//MODWIDTH 106kbps
	SetReg(fd,JREG_GSN,(GSNON<<4)|MODGSNON);	
	SetReg(fd,JREG_CWGSP,GSP);
	SetReg(fd,JREG_CONTROL,0x10);	//设置为读卡器模式
	SetReg(fd,JREG_RFCFG,(RXGAIN<<4)|0x08);
	SetReg(fd,JREG_RXTRESHOLD,(MINLEVEL<<4)|COLLLEVEL);
	ModifyReg(fd,JREG_TXAUTO,BIT6,SET);//Force 100ASK = 1
	Mifare_Clear_Crypto(fd);//Mifare卡片循环读卡操作时，需要在读卡前清除前次操作的认证标志
	SetReg(fd,JREG_DEMOD,0x0D);//解调器设置
	SetReg(fd,JREG_MANUALRCV,0x08);//接收器设置
	return;
}


void FM175XX_Initial_ReaderB(int fd)
{	
	SetReg(fd,JREG_TXMODE,BIT7|BIT1|BIT0);//BIT1~0 = 11 :ISO/IEC 14443B
	SetReg(fd,JREG_RXMODE,BIT7|BIT1|BIT0);//BIT1~0 = 11 :ISO/IEC 14443B
	SetReg(fd,JREG_TXAUTO,0);//Force 100ASK = 0
	SetReg(fd,JREG_MODWIDTH,0x26);//调制脉冲宽度设置
	SetReg(fd,JREG_RXTRESHOLD,0x84);//接收阈值设置
	SetReg(fd,JREG_GSN,(GSNON<<4)|MODGSNON);//NMOS 载波输出驱动设置，NMOS 调制输出驱动设置  
	SetReg(fd,JREG_CWGSP,GSP);//PMOS 载波输出驱动设置
	SetReg(fd,JREG_MODGSP,MODGSP);//PMOS 调制输出驱动设置
	SetReg(fd,JREG_CONTROL,0x10);//设置为读卡器模式
	SetReg(fd,JREG_RFCFG,(RXGAIN<<4)|0x08);//接收增益设置
	SetReg(fd,JREG_RFU1E,0);//TYPE B协议下SOF、EOF和EGT设置
	SetReg(fd,JREG_DEMOD,0x0D);//解调器设置
	SetReg(fd,JREG_MANUALRCV,0x48);//接收器设置
	return;
}


unsigned char ReaderA_Halt(int fd)
{
	unsigned char result;
	unsigned char outbuf[2],inbuf[1];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;	

	SetTxCRC(fd,SET);
	SetRxCRC(fd,SET);
	command_stauts.pSendBuf[0] = 0x50;
  command_stauts.pSendBuf[1] = 0x00;
  command_stauts.nBytesToSend = 2;
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 0;
  SetTimeOut(fd,5);//5ms	
	command_stauts.Cmd = CMD_TRANSMIT;	
  result = Command_Execute(fd,&command_stauts);
  return result;
}

unsigned char ReaderA_Wakeup(int fd)
{
	unsigned char result;
	unsigned char outbuf[1],inbuf[2];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;	
 	SetTxCRC(fd,RESET);
	SetRxCRC(fd,RESET);
	command_stauts.pSendBuf[0] = RF_CMD_WUPA;   //
	command_stauts.nBytesToSend = 1;//
	command_stauts.nBitsToSend = 7;
	command_stauts.nBytesToReceive = 2;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if((result == FM175XX_SUCCESS)&&(command_stauts.nBytesReceived == 2))
	{
		memcpy(Type_A.ATQA,command_stauts.pReceiveBuf,2);		
	}
	else
		result =  FM175XX_COMM_ERR;
	return result;
}

unsigned char ReaderA_Request(int fd)
{
	unsigned char result;
	unsigned char outbuf[1],inbuf[2];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;
 	SetTxCRC(fd,RESET);
	SetRxCRC(fd,RESET);
	command_stauts.pSendBuf[0] = RF_CMD_REQA;   //
	command_stauts.nBytesToSend = 1;//
	command_stauts.nBitsToSend = 7;
	command_stauts.nBytesToReceive = 2;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if((result == FM175XX_SUCCESS)&&(command_stauts.nBytesReceived == 2))
	{
		memcpy(Type_A.ATQA, command_stauts.pReceiveBuf,2);		
	}
	else
		result =  FM175XX_COMM_ERR;
	return result;
}

//*************************************
//函数  名：ReaderA_AntiColl
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：unsigned char:0:OK  others：错误
//*************************************
unsigned char ReaderA_AntiColl(int fd,unsigned char cascade_level)
{
	unsigned char result;
	unsigned char outbuf[2],inbuf[5];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;	
	SetTxCRC(fd,RESET);
	SetRxCRC(fd,RESET);
	if(cascade_level >2)
		return FM175XX_PARAM_ERR;
	command_stauts.pSendBuf[0] = RF_CMD_ANTICOL[cascade_level];   //防冲突等级
	command_stauts.pSendBuf[1] = 0x20;
	command_stauts.nBytesToSend = 2;						//发送长度：2
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 5;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	ModifyReg(fd,JREG_COLL,BIT7,SET);
	if((result == FM175XX_SUCCESS)&&(command_stauts.nBytesReceived == 5))
	{
		memcpy(Type_A.UID +(cascade_level*4),command_stauts.pReceiveBuf,4);
		memcpy(Type_A.BCC + cascade_level,command_stauts.pReceiveBuf + 4,1);
		if((Type_A.UID[cascade_level*4] ^  Type_A.UID[cascade_level*4 + 1] ^ Type_A.UID[cascade_level*4 + 2]^ Type_A.UID[cascade_level*4 + 3] ^ Type_A.BCC[cascade_level]) !=0)
				result = FM175XX_COMM_ERR;
	}
	return result;
}

//*************************************
//函数  名：ReaderA_Select
//入口参数：size:防冲突等级，包括0、1、2
//出口参数：unsigned char:0:OK  others：错误
//*************************************
unsigned char ReaderA_Select(int fd,unsigned char cascade_level)
{
	unsigned char result;
	unsigned char outbuf[7],inbuf[1];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;	
	SetTxCRC(fd,SET);
	SetRxCRC(fd,SET);
	if(cascade_level > 2)							 //最多三重防冲突
		return FM175XX_PARAM_ERR;
	*(command_stauts.pSendBuf+0) = RF_CMD_ANTICOL[cascade_level];   //防冲突等级
	*(command_stauts.pSendBuf+1) = 0x70;   					//
	*(command_stauts.pSendBuf+2) = Type_A.UID[4*cascade_level];
	*(command_stauts.pSendBuf+3) = Type_A.UID[4*cascade_level+1];
	*(command_stauts.pSendBuf+4) = Type_A.UID[4*cascade_level+2];
	*(command_stauts.pSendBuf+5) = Type_A.UID[4*cascade_level+3];
	*(command_stauts.pSendBuf+6) = Type_A.BCC[cascade_level];
	command_stauts.nBytesToSend = 7;
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 1;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if((result == FM175XX_SUCCESS)&&(command_stauts.nBytesReceived == 1))
		Type_A.SAK [cascade_level] = *(command_stauts.pReceiveBuf);
	else
		result = FM175XX_COMM_ERR;
	return result;
}



void SetRf(int fd,unsigned char mode)
{
	if(mode == 0)	
		{
		ModifyReg(fd,JREG_TXCONTROL,BIT0|BIT1,RESET);
		}
	if(mode == 1)	
		{
		ModifyReg(fd,JREG_TXCONTROL,BIT0,SET);
		ModifyReg(fd,JREG_TXCONTROL,BIT1,RESET);	
		}
	if(mode == 2)	
		{
		ModifyReg(fd,JREG_TXCONTROL,BIT0,RESET);
		ModifyReg(fd,JREG_TXCONTROL,BIT1,SET);	
		}
	if(mode == 3)	
		{
		ModifyReg(fd,JREG_TXCONTROL,BIT0|BIT1,SET);
		}
		usleep(10000);
}

unsigned char ReaderA_CardActivate(int fd)
{
unsigned char  result,cascade_level;	
		result = ReaderA_Wakeup(fd);//
		if (result != FM175XX_SUCCESS)
			return FM175XX_COMM_ERR;
				
			if 	((Type_A.ATQA[0]&0xC0)==0x00)	//1重UID		
				Type_A.CASCADE_LEVEL = 1;
			
			if 	((Type_A.ATQA[0]&0xC0)==0x40)	//2重UID			
				Type_A.CASCADE_LEVEL = 2;		

			if 	((Type_A.ATQA[0]&0xC0)==0x80)	//3重UID
				Type_A.CASCADE_LEVEL = 3;				
			
			for (cascade_level = 0;cascade_level < Type_A.CASCADE_LEVEL;cascade_level++)
				{
					result = ReaderA_AntiColl(fd,cascade_level);//
					if (result != FM175XX_SUCCESS)
						return FM175XX_COMM_ERR;
					
					result=ReaderA_Select(fd,cascade_level);//
					if (result != FM175XX_SUCCESS)
						return FM175XX_COMM_ERR;
				}						
			
		return result;
}

unsigned char ReaderB_Wakeup(int fd)
{
	unsigned char result;
	unsigned char outbuf[3],inbuf[12];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;	

 	SetTxCRC(fd,SET);
	SetRxCRC(fd,SET);
	command_stauts.pSendBuf[0] = 0x05;  //APf
	command_stauts.pSendBuf[1] = 0x00;   //AFI (00:for all cards)
	command_stauts.pSendBuf[2] = 0x08;   //PARAM(REQB,Number of slots =0)
	command_stauts.nBytesToSend = 3;//
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 12;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if((result == FM175XX_SUCCESS)&&(command_stauts.nBytesReceived == 12))
	{
		memcpy(Type_B.ATQB, command_stauts.pReceiveBuf,12);	
		memcpy(Type_B.PUPI,Type_B.ATQB + 1,4);
		memcpy(Type_B.APPLICATION_DATA,Type_B.ATQB + 6,4);
		memcpy(Type_B.PROTOCOL_INF,Type_B.ATQB + 10,3);
	}
	else
		result = FM175XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_Request(int fd)
{
	unsigned char result;
	unsigned char outbuf[3],inbuf[12];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;
 	SetTxCRC(fd,SET);
	SetRxCRC(fd,SET);
	command_stauts.pSendBuf[0] = 0x05;  //APf
	command_stauts.pSendBuf[1] = 0x00;   //AFI (00:for all cards)
	command_stauts.pSendBuf[2] = 0x00;   //PARAM(REQB,Number of slots =0)
	command_stauts.nBytesToSend = 3;//
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 12;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if((result == FM175XX_SUCCESS)&&(command_stauts.nBytesReceived == 12))
	{
		memcpy(Type_B.ATQB, command_stauts.pReceiveBuf,12);	
		memcpy(Type_B.PUPI,Type_B.ATQB + 1,4);
		memcpy(Type_B.APPLICATION_DATA,Type_B.ATQB + 6,4);
		memcpy(Type_B.PROTOCOL_INF,Type_B.ATQB + 10,3);
	}
	else
		result = FM175XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_Attrib(int fd)
{
	unsigned char result;
	unsigned char outbuf[9],inbuf[1];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;
 	SetTxCRC(fd,SET);
	SetRxCRC(fd,SET);
	command_stauts.pSendBuf[0] = 0x1D;  //
	command_stauts.pSendBuf[1] = Type_B.PUPI[0];   //
	command_stauts.pSendBuf[2] = Type_B.PUPI[1];   //
	command_stauts.pSendBuf[3] = Type_B.PUPI[2];   //
	command_stauts.pSendBuf[4] = Type_B.PUPI[3];   //	
	command_stauts.pSendBuf[5] = 0x00;  //Param1
	command_stauts.pSendBuf[6] = 0x08;  //01--24,08--256
	command_stauts.pSendBuf[7] = 0x01;  //COMPATIBLE WITH 14443-4
	command_stauts.pSendBuf[8] = 0x01;  //CID:01 
	command_stauts.nBytesToSend = 9;//
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 1;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if(result == FM175XX_SUCCESS)
	{
		Type_B.LEN_ATTRIB = command_stauts.nBytesReceived;
		memcpy(Type_B.ATTRIB, command_stauts.pReceiveBuf,Type_B.LEN_ATTRIB);		
	}
	else
		result = FM175XX_COMM_ERR;
	return result;
}

unsigned char ReaderB_GetUID(int fd)
{
		unsigned char result;
	unsigned char outbuf[5],inbuf[10];
	command_struct command_stauts;
	command_stauts.pSendBuf = outbuf;
	command_stauts.pReceiveBuf = inbuf;
 	SetTxCRC(fd,SET);
	SetRxCRC(fd,SET);
	command_stauts.pSendBuf[0] = 0x00;  //
	command_stauts.pSendBuf[1] = 0x36;   //
	command_stauts.pSendBuf[2] = 0x00;   //
	command_stauts.pSendBuf[3] = 0x00;   //
	command_stauts.pSendBuf[4] = 0x08;   //	
	
	command_stauts.nBytesToSend = 5;//
	command_stauts.nBitsToSend = 0;
	command_stauts.nBytesToReceive = 10;
	SetTimeOut(fd,1);//1ms	
	command_stauts.Cmd = CMD_TRANSCEIVE;
	result = Command_Execute(fd,&command_stauts);
	if(result == FM175XX_SUCCESS)
	{		
		memcpy(Type_B.UID, command_stauts.pReceiveBuf,8);		
	}
	else
		result = FM175XX_COMM_ERR;
	return result;
}

unsigned char FM175XX_Polling(int fd,unsigned char *polling_card)
{
unsigned char result;
	*polling_card = 0;
	FM175XX_Initial_ReaderA(fd);
	SetRf(fd,3);	
	result = ReaderA_Wakeup(fd);//
		if (result == FM175XX_SUCCESS)	
			*polling_card |= BIT0;
	FM175XX_Initial_ReaderB(fd);
	result = ReaderB_Wakeup(fd);//
	SetRf(fd,0);	
		if (result == FM175XX_SUCCESS)
			*polling_card |= BIT1;
if (*polling_card !=0)		
	return SUCCESS;	
else
	return ERROR;
}

