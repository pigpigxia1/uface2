/****************************************Copyright (c)****************************************************
**                            Guangzhou ZLGMCU Technology Co., LTD
**
**                                 http://www.zlgmcu.com
**
**      广州周立功单片机科技有限公司所提供的所有服务内容旨在协助客户加速产品的研发进度，在服务过程中所提供
**  的任何程序、文档、测试结果、方案、支持等资料和信息，都仅供参考，客户有权不使用或自行参考修改，本公司不
**  提供任何的完整性、可靠性等保证，若在客户使用过程中因任何原因造成的特别的、偶然的或间接的损失，本公司不
**  承担任何责任。
**                                                                        ——广州周立功单片机科技有限公司
**
**--------------File Info---------------------------------------------------------------------------------
** File name:           type_a.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        ISO/IEC144443A卡相关操作
**
**--------------------------------------------------------------------------------------------------------
*/

#include <stdint.h>
#include "fm175xx.h"
#include "Utility.h"
#include <string.h>    

#include "uart.h"
#include "gpio.h"
#include <unistd.h>

//#define TRUE 1
//#define FALSE -1

unsigned char FM175XX_IRQ;

#define MI_NOTAGERR 0xEE
#define MAXRLEN 64

#define REST   229

/*
void Delay100us(int time)
{
	usleep(100*time);
}*/

void rest_init()
{
	gpio_export(REST);
	gpio_setDir(REST,1);
	gpio_setValue(REST,1);
}


/*********************************************************************************************************
** Function name:       pcd_Init
** Descriptions:        MCU初始化函数、包括SPI和UART的初始化
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      
*********************************************************************************************************/
void pcd_Init(void)
{
    //SPI_Init();
    //CD_CfgTPD();                                                        /* 配置复位管脚                 */
	
	rest_init();
	uart_init(9600,8,'N',1);
}

static unsigned char spi_GetReg(unsigned char Reg)
{
	unsigned char addr;
	
	addr = Reg | 0x80;
	uart_senddat(addr);
	
	return uart_read();
	
}

static unsigned char spi_SetReg(unsigned char Reg, unsigned char value)
{
	unsigned char addr;
	
	addr = Reg & 0x7F;	
	uart_senddat(addr);
	uart_read();
	uart_senddat(value);
	return 0;
}
/*********************************************************************************************************
** Function name:       MCU_TO_PCD_TEST
** Descriptions:        MCU与读卡芯片通信测试函数
**                      不同的读卡芯片的选取用于测试的寄存器不一样，需要更具具体芯片而确定目标寄存器
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      TRUE---测试通过    FALSE----测试失败
*********************************************************************************************************/
uint8_t MCU_TO_PCD_TEST(void)
{
    volatile uint8_t ucRegVal;
    pcd_RST();                                                      /* 硬件复位                         */
    ucRegVal = spi_GetReg(ControlReg);
    spi_SetReg(ControlReg, 0x10);                                   /* 启动读写器模式                   */
    ucRegVal = spi_GetReg(ControlReg);
    spi_SetReg(GsNReg, 0xF0 | 0x04);                                /* CWGsN = 0xF; ModGsN = 0x4        */
    ucRegVal = spi_GetReg(GsNReg);
    if(ucRegVal != 0xF4)                                            /* 验证接口正确                     */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       pcd_RST
** Descriptions:        PCD控制PICC硬件复位函数
** input parameters:    N/A
** output parameters:   N/A
** Returned value:      寄存器数值
*********************************************************************************************************/
void pcd_RST(void)
{
    /*CD_SetTPD();
    Delay100us(3);
    CD_ClrTPD();                                                        // 复位管脚拉低                 
    Delay100us(5);
    CD_SetTPD();                                                        // 再拉高                       
    Delay100us(10);*/
	
	gpio_setValue(REST,0);
	usleep(10000);
	gpio_setValue(REST,1);
	usleep(10000);
	
	return ;
}


/*********************************************************************************************************
** Function name:       Read_Reg
** Descriptions:        读取寄存器                
** input parameters:    reg_add:寄存器数值
** output parameters:   N/A
** Returned value:      寄存器数值
*********************************************************************************************************/
unsigned char Read_Reg(unsigned char reg_add)
{
    unsigned char  reg_value;       
    reg_value=spi_GetReg(reg_add);
    return reg_value;
}

/*********************************************************************************************************
** Function name:       Read_Reg_All
** Descriptions:        读取全部寄存器                
** input parameters:    reg_value:寄存器数值
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Read_Reg_All(unsigned char *reg_value)
{
    unsigned char data i;
    for (i=0;i<64;i++)       
        *(reg_value+i) = spi_GetReg(i);
    return TRUE;
}


/*********************************************************************************************************
** Function name:       Write_Reg
** Descriptions:        写寄存器操作                
** input parameters:    reg_add:寄存器地址
**                      reg_value:寄存器数值
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Write_Reg(unsigned char reg_add,unsigned char reg_value)
{
    spi_SetReg(reg_add,reg_value);
    return TRUE;
}


static void SPIRead_Sequence(unsigned char sequence_length,unsigned char ucRegAddr,unsigned char *reg_value)    
{
    uint8_t i;
    if (sequence_length==0)
		return;
    
    for(i=0;i<sequence_length;i++) {
        *(reg_value+i) = spi_GetReg(ucRegAddr);
    }
   
    return;

}

static void SPIWrite_Sequence(unsigned char sequence_length,unsigned char ucRegAddr,unsigned char *reg_value)
{
    
    uint8_t i;
    if(sequence_length==0)
        return;
    for(i=0;i<sequence_length;i++) {
        spi_SetReg(ucRegAddr, *(reg_value+i));
    }
 
    return ;    
}

/*********************************************************************************************************
** Function name:       Read_FIFO
** Descriptions:        读出FIFO的数据         
** input parameters:    length:读取数据长度
**                      *fifo_data:数据存放指针
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
void Read_FIFO(unsigned char length,unsigned char *fifo_data)
{     
    SPIRead_Sequence(length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Write_FIFO
** Descriptions:        写入FIFO         
** input parameters:    length:读取数据长度
**                      *fifo_data:数据存放指针
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
void Write_FIFO(unsigned char length,unsigned char *fifo_data)
{
    SPIWrite_Sequence(length,FIFODataReg,fifo_data);
    return;
}


/*********************************************************************************************************
** Function name:       Clear_FIFO
** Descriptions:        清空FIFO              
** input parameters:   
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Clear_FIFO(void)
{
    Set_BitMask(FIFOLevelReg,0x80);                                     /* 清除FIFO缓冲                 */
    if ( spi_GetReg(FIFOLevelReg) == 0 )
        return TRUE;
    else
        return FALSE;
}


/*********************************************************************************************************
** Function name:       Set_BitMask
** Descriptions:        置位寄存器操作    
** input parameters:    reg_add，寄存器地址
**                      mask，寄存器清除位
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Set_BitMask(unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=spi_SetReg(reg_add,Read_Reg(reg_add) | mask);                /* set bit mask                 */
    return result;
}

/*********************************************************************************************************
** Function name:       Clear_BitMask
** Descriptions:        清除位寄存器操作
** input parameters:    reg_add，寄存器地址
**                      mask，寄存器清除位
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Clear_BitMask(unsigned char reg_add,unsigned char mask)
{
    unsigned char result;
    result=Write_Reg(reg_add,Read_Reg(reg_add) & ~mask);                /* clear bit mask               */
    return result;
}


/*********************************************************************************************************
** Function name:       Set_RF
** Descriptions:        设置射频输出
** input parameters:    mode，射频输出模式
**                      0，关闭输出
**                      1，仅打开TX1输出
**                      2，仅打开TX2输出
**                      3，TX1，TX2打开输出，TX2为反向输出
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Set_Rf(unsigned char mode)
{
    unsigned char result;
    if( (Read_Reg(TxControlReg)&0x03) == mode )
        return TRUE;
    if( mode == 0 )
        result = Clear_BitMask(TxControlReg,0x03);                      /* 关闭TX1，TX2输出             */
    if( mode== 1 )
        result = Clear_BitMask(TxControlReg,0x01);                      /* 仅打开TX1输出                */
    if( mode == 2)
        result = Clear_BitMask(TxControlReg,0x02);                      /* 仅打开TX2输出                */
    if (mode==3)
        result=Set_BitMask(TxControlReg,0x03);                          /* 打开TX1，TX2输出             */
    Delay100us(1000);
    return result;
}
 
/*********************************************************************************************************
** Function name:       Pcd_Comm
** Descriptions:        读卡器通信 不利用IRQ管脚的情况
** input parameters:    Command:通信操作命令
**                      pInData:发送数据数组
**                      InLenByte:发送数据数组字节长度
**                      pOutData:接收数据数组
**                      pOutLenBit:接收数据的位长度
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Pcd_Comm(    unsigned char Command, 
                         unsigned char *pInData, 
                         unsigned char InLenByte,
                         unsigned char *pOutData, 
                         unsigned int *pOutLenBit)
{
    uint8_t status  = FALSE;
    uint8_t irqEn   = 0x00;                                             /* 使能的中断                   */
    uint8_t waitFor = 0x00;                                             /* 等待的中断                   */
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;
    Write_Reg(ComIrqReg, 0x7F);                                         /* 清楚IRQ标记                  */
    Write_Reg(TModeReg,0x80);                                           /* 设置TIMER自动启动            */
    switch (Command) {
    case MFAuthent:                                                     /* Mifare认证                   */
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case Transceive:                                       /* 发送FIFO中的数据到天线，传输后激活接收电路*/
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }
   
    Write_Reg(ComIEnReg, irqEn | 0x80);
   // Clear_BitMask(ComIrqReg, 0x80);
    Write_Reg(CommandReg, Idle);
    Set_BitMask(FIFOLevelReg, 0x80);
    
    for (i=0; i < InLenByte; i++) {
        Write_Reg(FIFODataReg, pInData[i]);
    }
    Write_Reg(CommandReg, Command);

    if (Command == Transceive) {
        Set_BitMask(BitFramingReg, 0x80);
    }

    i = 3000;                                              /* 根据时钟频率调整，操作M1卡最大等待时间25ms*/

    do {
        n = Read_Reg(ComIrqReg);
        i--;                                                            /* i==0表示延时到了             */
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor));            /* n&0x01!=1表示PCDsettimer时间未到 */
                                                                        /* n&waitFor!=1表示指令执行完成 */
    Clear_BitMask(BitFramingReg, 0x80);
    if (i != 0) {
        if(!(Read_Reg(ErrorReg) & 0x1B)) {
            status = TRUE;
            if (n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            if (Command == Transceive) {
                n = Read_Reg(FIFOLevelReg);
                lastBits = Read_Reg(ControlReg) & 0x07;
                if (lastBits) {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                } else {
                    *pOutLenBit = n * 8;
                }
                if (n == 0) {
                    n = 1;
                }
                if (n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++) {
                    pOutData[i] = Read_Reg(FIFODataReg);
                }
            }
        } else {
            status = FALSE;
        }
    }
       Clear_BitMask(BitFramingReg,0x80);//关闭发送
    return status;
}


/*********************************************************************************************************
** Function name:       Pcd_SetTimer
** Descriptions:        设置接收延时
** input parameters:    delaytime，延时时间（单位为毫秒）  
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Pcd_SetTimer(unsigned long delaytime)
{
    unsigned long  TimeReload;
    unsigned int Prescaler;

    Prescaler=0;
    TimeReload=0;
    while(Prescaler<0xfff) {
        TimeReload = ((delaytime*(long)13560)-1)/(Prescaler*2+1);
        if( TimeReload<0xffff)
            break;
        Prescaler++;
    }
    TimeReload=TimeReload&0xFFFF;
    Set_BitMask(TModeReg,Prescaler>>8);
    Write_Reg(TPrescalerReg,Prescaler&0xFF);                    
    Write_Reg(TReloadMSBReg,TimeReload>>8);
    Write_Reg(TReloadLSBReg,TimeReload&0xFF);
    return TRUE;
}

/*********************************************************************************************************
** Function name:       Pcd_ConfigISOType
** Descriptions:        配置ISO14443A/B协议
** input parameters:    type = 0：ISO14443A协议；
**                                 type = 1，ISO14443B协议；   
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Pcd_ConfigISOType(unsigned char data type)
{
    if (type == 0)   {                                                  /* 配置为ISO14443_A             */
        Set_BitMask(ControlReg, 0x10);                                /* ControlReg 0x0C 设置reader模式 */
        Set_BitMask(TxAutoReg, 0x40);                                  /* TxASKReg 0x15 设置100%ASK有效 */
        Write_Reg(TxModeReg, 0x00);                 /* TxModeReg 0x12 设置TX CRC无效，TX FRAMING =TYPE A */
        Write_Reg(RxModeReg, 0x00);                 /* RxModeReg 0x13 设置RX CRC无效，RX FRAMING =TYPE A */
    }
    if (type == 1)   {                                                  /* 配置为ISO14443_B           */
        Write_Reg(ControlReg,0x10);
        Write_Reg(TxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(RxModeReg,0x83);                                      /* BIT1~0 = 2'b11:ISO/IEC 14443B */
        Write_Reg(TxAutoReg,0x00);
        Write_Reg(RxThresholdReg,0x55);
        Write_Reg(RFCfgReg,0x48);
        Write_Reg(TxBitPhaseReg,0x87);                                  /* 默认值                         */
        Write_Reg(GsNReg,0x83);    
        Write_Reg(CWGsPReg,0x30);
        Write_Reg(GsNOffReg,0x38);
        Write_Reg(ModGsPReg,0x20);
    
    }
    Delay100us(30);
    return TRUE;
}


/*********************************************************************************************************
** Function name:       FM175X_SoftReset
** Descriptions:        FM175xx软件复位
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char  FM175X_SoftReset(void)
{    
    Write_Reg(CommandReg,SoftReset);
    return    Set_BitMask(ControlReg,0x10);                               /* 17520初始值配置              */
}

/*********************************************************************************************************
** Function name:       FM175X_HardReset
** Descriptions:        FM175xx硬件复位
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char FM175X_HardReset(void)
{    
    gpio_setValue(REST,0);
	usleep(10000);
	gpio_setValue(REST,1);
	usleep(10000);
	
    return TRUE;
}

    
/*********************************************************************************************************
** Function name:       FM175X_SoftPowerdown
** Descriptions:        硬件低功耗操作
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char FM175X_SoftPowerdown(void)
{
    if(Read_Reg(CommandReg)&0x10) {
       Clear_BitMask(CommandReg,0x10);                                 /* 退出低功耗模式               */
       return FALSE;
    }
    else
        Set_BitMask(CommandReg,0x10);                                       /* 进入低功耗模式               */
    return TRUE;
}

/*********************************************************************************************************
** Function name:       FM175X_HardPowerdown
** Descriptions:        硬件低功耗操作
** input parameters:    N/A       
** output parameters:   N/A
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char FM175X_HardPowerdown(void)
{    
    //NPD=~NPD;
    //if(NPD==1)                                                          /* 进入低功耗模式               */
    return TRUE;                                
//    else
        //return FALSE;                                                     /* 退出低功耗模式               */
}

/*********************************************************************************************************
** Function name:       Read_Ext_Reg
** Descriptions:        读取扩展寄存器
** input parameters:    reg_add，寄存器地址；         
** output parameters:   reg_value，寄存器数值
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Read_Ext_Reg(unsigned char reg_add)
{
     Write_Reg(0x0F,0x80+reg_add);
     return Read_Reg(0x0F);
}

/*********************************************************************************************************
** Function name:       Write_Ext_Reg
** Descriptions:        写入扩展寄存器
** input parameters:    reg_add，寄存器地址；
**                      reg_value，寄存器数值
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Write_Ext_Reg(unsigned char reg_add,unsigned char reg_value)
{
    Write_Reg(0x0F,0x40+reg_add);
    return (Write_Reg(0x0F,0xC0+reg_value));
}


