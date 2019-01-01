
#ifndef _FM175XX_H
#define _FM175XX_H

#ifndef ERROR
#define ERROR 0
#endif
#ifndef SUCCESS
#define SUCCESS 1
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef RESET
#define RESET 0
#endif
#ifndef SET
#define SET 1
#endif

#define BIT0               0x01
#define BIT1               0x02
#define BIT2               0x04
#define BIT3               0x08
#define BIT4               0x10
#define BIT5               0x20
#define BIT6               0x40
#define BIT7               0x80

typedef unsigned char uchar;
typedef unsigned char u8;

extern void FM175XX_HardReset(void);
extern unsigned char FM175XX_SoftReset(int fd);
//extern void FM175XX_HPD(unsigned char mode);
extern void GetReg(int fd,unsigned char Reg,unsigned char* regdata);
extern void SetReg(int fd,unsigned char Reg,unsigned char regdata);
extern void ModifyReg(int fd,unsigned char addr,unsigned char mask,unsigned char set);
extern void SPI_Write_FIFO(int fd,unsigned char reglen,unsigned char* regbuf); //SPI接口连续写FIFO 
extern void SPI_Read_FIFO(int fd,unsigned char reglen,unsigned char* regbuf);  //SPI接口连续读FIFO
extern unsigned char FM175XX_Get_FIFOlength(int fd);
extern void Clear_FIFO(int fd);
extern void GetReg_Ext(int fd,unsigned char ExtRegAddr,unsigned char* ExtRegData);
extern void SetReg_Ext(int fd,unsigned char ExtRegAddr,unsigned char ExtRegData);
extern void ModifyReg_Ext(int fd,unsigned char ExtRegAddr,unsigned char mask,unsigned char set);

unsigned char Set_EMV_Enhance(int fd,unsigned char set);


#endif





