
#ifndef _CARD_EMULATE_API_H_
#define _CARD_EMULATE_API_H_


static const unsigned char CARD_ATS[5] ={0x05,0x72,0x76,0x60,0x02};
static unsigned char CARD_UID_INFO[25] = {
																								0x10,0x04,																//SENS_RES(2 bytes)
																								0x01,0x02,0x03,														//NFCID1(3 bytes)
																								0x00,																			//SEL_RES(1 byte)
																								0x01,0xFE,																//polling response 
																								0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,						//NFCID2(6 bytes)
																								0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,	//pad (8 bytes)
																								0xAA,0xBB,																//system code (2 bytes)
																								0xFE																			//NFCID3 (1 byte)
																								};

#define FM175XX_Card_Buf_Len  64	   //卡数据缓冲区长度定义
			
struct CARD_STRUCT
{
	unsigned char IRQ;	
	unsigned char RecvFlag;//接收完成标志
	unsigned char RecvLen;
  unsigned char SendLen;
  unsigned char RecvBuf[FM175XX_Card_Buf_Len];			//FM175XX卡数据接收缓冲区
  unsigned char SendBuf[FM175XX_Card_Buf_Len];			//FM175XX卡数据发送缓冲区
	
};	

extern struct CARD_STRUCT CARD;

extern void Card_Emulate_Init(int fd);
extern unsigned char Card_Emulate_Config(int fd,unsigned char *configbuf);
extern void Card_Emulate_AutoColl(int fd); 
extern unsigned char Card_Emulate_IRQ_Event(int fd,unsigned char tag_com_irq,unsigned char tag_div_irq);
extern unsigned char Card_Emulate_TX(int fd,unsigned char ilen,unsigned char *ibuf);
extern void Card_Emulate_RX(int fd,unsigned char *rece_buf,unsigned char *rece_len);
																							

#endif

