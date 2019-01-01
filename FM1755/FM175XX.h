
#ifndef _FM175XX_H
#define _FM175XX_H
//#include "stm32f10x_map.h"


#define SUCCESS 0
#define ERROR   -1

#define BIT0               0x01
#define BIT1               0x02
#define BIT2               0x04
#define BIT3               0x08
#define BIT4               0x10
#define BIT5               0x20
#define BIT6               0x40
#define BIT7               0x80 


//typedef uchar u8

extern void uart_senddat(unsigned char regdata);
extern unsigned char uart_read();
void rest_init();
extern void FM175XX_HardReset(void);
extern unsigned char FM175XX_SoftReset(void);
extern void FM175XX_HPD(unsigned char mode);
extern void GetReg(unsigned char addr,unsigned char* regdata);
extern void SetReg(unsigned char addr,unsigned char regdata);
extern void ModifyReg(unsigned char addr,unsigned char mask,unsigned char set);
extern void SPI_Write_FIFO(unsigned char reglen,unsigned char* regbuf); //SPI接口连续写FIFO 
extern void SPI_Read_FIFO(unsigned char reglen,unsigned char* regbuf);  //SPI接口连续读FIFO

extern void Clear_FIFO(void);
extern void GetReg_Ext(unsigned char ExtRegAddr,unsigned char* ExtRegData);
extern void SetReg_Ext(unsigned char ExtRegAddr,unsigned char ExtRegData);
extern void ModifyReg_Ext(unsigned char ExtRegAddr,unsigned char mask,unsigned char set);
#endif





