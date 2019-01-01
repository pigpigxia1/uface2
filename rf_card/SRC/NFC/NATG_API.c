/************************************************************
 Shanghai Fudan Microelectronics Group Company Limited
 
 file name:NTAG_API.c
 
 version:

 project:FM175XX
*************************************************************/

#include "string.h"


#include "fm175xx.h"
#include "FM175XX_REG.h"
//#include "READER_API.h"
#include "CARD_EMULATE_API.h"
#include "NTAG_API.h"

#define Block_Capacity 64
 unsigned char Ntag_Block[Block_Capacity][4]= 
																	{{0x08,0x01,0x02,0x03},//Block 0
																	 {0x00,0x00,0x00,0x00},//Block 1
																	 {0x00,0x00,0x00,0x00},//Block 2
																	 {0x00,0x00,0x00,0x00},//Block 3
																	 {0x00,0x00,0x00,0x00},//Block 4
																	 {0x00,0x00,0x00,0x00},//Block 5
																	 {0x00,0x00,0x00,0x00},//Block 6
																	 {0x00,0x00,0x00,0x00},//Block 7
																	 {0x00,0x00,0x00,0x00},//Block 8
																	 {0x00,0x00,0x00,0x00},//Block 9
																	 {0x00,0x00,0x00,0x00},//Block 10
																	 {0x00,0x00,0x00,0x00},//Block 11
																	 {0x00,0x00,0x00,0x00},//Block 12
																	 {0x00,0x00,0x00,0x00},//Block 13
																	 {0x00,0x00,0x00,0x00},//Block 14
																	 {0xA1,0xA2,0xA3,0xA4},//Block 15
																	 {0x00,0x00,0x00,0x00},//Block 16
																	 {0x00,0x00,0x00,0x00},//Block 17
																	 {0x00,0x00,0x00,0x00},//Block 18
																	 {0x00,0x00,0x00,0x00},//Block 19
																	 {0x00,0x00,0x00,0x00},//Block 20
																	 {0x00,0x00,0x00,0x00},//Block 21
																	 {0x00,0x00,0x00,0x00},//Block 22
																	 {0x00,0x00,0x00,0x00},//Block 23
																	 {0x00,0x00,0x00,0x00},//Block 24
																	 {0x00,0x00,0x00,0x00},//Block 25
																	 {0x00,0x00,0x00,0x00},//Block 26
																	 {0x00,0x00,0x00,0x00},//Block 27
																	 {0x00,0x00,0x00,0x00},//Block 28
																	 {0x00,0x00,0x00,0x00},//Block 29
																	 {0x00,0x00,0x00,0x00},//Block 30
																	 {0xB1,0xB2,0xB3,0xB4},//Block 31
																	 {0x00,0x00,0x00,0x00},//Block 32
																	 {0x00,0x00,0x00,0x00},//Block 33
																	 {0x00,0x00,0x00,0x00},//Block 34
																	 {0x00,0x00,0x00,0x00},//Block 35
																	 {0x00,0x00,0x00,0x00},//Block 36
																	 {0x00,0x00,0x00,0x00},//Block 37
																	 {0x00,0x00,0x00,0x00},//Block 38
																	 {0x00,0x00,0x00,0x00},//Block 39
																	 {0x00,0x00,0x00,0x00},//Block 40
																	 {0x00,0x00,0x00,0x00},//Block 41
																	 {0x00,0x00,0x00,0x00},//Block 42
																	 {0x00,0x00,0x00,0x00},//Block 43
																	 {0x00,0x00,0x00,0x00},//Block 44
																	 {0x00,0x00,0x00,0x00},//Block 45
																	 {0x00,0x00,0x00,0x00},//Block 46
																	 {0xC1,0xC2,0xC3,0xC4},//Block 47
																	 {0x00,0x00,0x00,0x00},//Block 48
																	 {0x00,0x00,0x00,0x00},//Block 49
																	 {0x00,0x00,0x00,0x00},//Block 50
																	 {0x00,0x00,0x00,0x00},//Block 51
																	 {0x00,0x00,0x00,0x00},//Block 52
																	 {0x00,0x00,0x00,0x00},//Block 53
																	 {0x00,0x00,0x00,0x00},//Block 54
																	 {0x00,0x00,0x00,0x00},//Block 55
																	 {0x00,0x00,0x00,0x00},//Block 56
																	 {0x00,0x00,0x00,0x00},//Block 57
																	 {0x00,0x00,0x00,0x00},//Block 58
																	 {0x00,0x00,0x00,0x00},//Block 59
																	 {0x00,0x00,0x00,0x00},//Block 60
																	 {0x00,0x00,0x00,0x00},//Block 61
																	 {0x00,0x00,0x00,0x00},//Block 62
																	 {0xD1,0xD2,0xD3,0xD3}};//Block 63
																	 
																	 
																	 
																	 


/*********************************************************************************************************
** 函数名称:	NTAG_Ins_Event
** 函数功能:	FM175XX处理NTAG指令流程
** 输入参数:		无
** 输出参数:		无
** 返回值:			无
*********************************************************************************************************/
void NTAG_Ins_Event(int fd,unsigned char *rBuf,unsigned char rlength)
{
	unsigned char ins,param,block,offset;
	
	if(rlength == 0)
		return;
	ins = rBuf[0];
	param = rBuf[1];
	CARD.SendLen = 0;	
	/*Uart_Send_Msg(">Rece = ");	Uart_Send_Hex(rBuf,rlength);	Uart_Send_Msg("\r\n");	*/
	
	Clear_BitMask(fd,JREG_COMMIEN,JBIT_RXI | JBIT_ERRI);//关闭接收中断	
	
		
	if((ins == 0x30)&&(param < Block_Capacity)&&(rlength==2))//读块
		{
			offset = 0;						
			for(block = param; block< (param + 4); block++)
				{
							if(block < Block_Capacity)
								memcpy(CARD.SendBuf + (4*offset),Ntag_Block[block],4);					
							else
								memcpy(CARD.SendBuf+ (4*offset),Ntag_Block[block - Block_Capacity],4);	
							offset++;
				}	
			CARD.SendLen = 16;
		}	
		
	if((ins == 0x50)&&(param == 0x00))//Halt指令
		{
				//Uart_Send_Msg("Halt \r\n ");
				FM175XX_SoftReset(fd);
			  Card_Emulate_Init(fd);
				Card_Emulate_Config(fd,CARD_UID_INFO);
				Set_BitMask(JREG_MIFARE,BIT2);
				Card_Emulate_AutoColl(fd);
			return;
		}
	
	Card_Emulate_TX(fd,CARD.SendLen,CARD.SendBuf);//发送应答数据
	SetReg(fd,JREG_COMMAND,CMD_RECEIVE);	 //进入接收状态	
	SetReg(fd,JREG_COMMIEN,JBIT_IRQINV|JBIT_RXI|JBIT_ERRI);//打开接收中断	
	return;
}

void NTAG_Event(int fd)
{
	if(CARD.RecvFlag == FALSE)
		return;
	CARD.RecvFlag = FALSE;	
	Card_Emulate_RX(fd,CARD.RecvBuf,&CARD.RecvLen);//读卡指令接收			
	NTAG_Ins_Event(fd,CARD.RecvBuf, CARD.RecvLen);//读卡指令处理	
	SetReg(fd,JREG_COMMIRQ,JBIT_RXI | JBIT_HIALERTI );		//清除中断
	Set_BitMask(JREG_COMMIEN,JBIT_RXI | JBIT_HIALERTI);						
							
	return;
}
