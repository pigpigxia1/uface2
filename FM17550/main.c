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
** File name:           main.c
** Last modified Date:  2016-3-4
** Last Version:        V1.00
** Descriptions:        用于演示读卡功能
**
**--------------------------------------------------------------------------------------------------------
*/


#include "fm175xx.h"
#include "type_a.h"
#include "type_b.h"
#include "Utility.h"
#include "mifare_card.h"

#include "string.h"

#include <stdio.h>



/*
** 测试函数
*/
extern void TyteA_Test(void);
extern void TyteB_Test(void);
extern void MifareCard_Test (uint8_t command);

/*********************************************************************************************************
** Function name:       UartCommand()
** Descriptions:        串口接收到数据，执行测试命令
** input parameters:    无
** output parameters:   无
** Returned value:      无
*********************************************************************************************************/
void UartCommand(void)
{
    
  /*  if(GucRcvNew ==1)  {                                                // 串口接收到数据               
        MifareCard_Test( Recv_Buff[0] );                                // 第一个串口数据位命令类型     
        GucRcvNew = 0;                                                  // 清空命令参数                 
        memset(Recv_Buff,0x00,Maxlength);                               // 数据请0                      
    }*/
}

/*********************************************************************************************************
** 程序功能：Demo板通过串口接收命令，根据不同的命令编号执行不同的操作----波特率固定115200
** 例程功能：通过CON引脚的电平进入两种不同模式：  CON=1-----模式1（卡片自动检测模式）---绿灯常亮
**     此模式下，卡片会一直检测卡片，当有卡片靠近天线时，会读取卡片的ID号并通过串口打印ID号
**     输出格式波特率115200，字符输出
**                                                CON=0-----模式2（串口命令控制模式，介绍如下）
** 注意：进入循环寻卡模式后，Demo板就不能接受命令了，要想返回前面的命令模式，则需要重新上电
**
*********************************************************************************************************/
int main(void)
{
    volatile unsigned char statues;
    //SystemInit();                                                       /* MCU系统初始化                */
    Delay100us(1000);
    //GPIOInit();                                                         /* GPIO初始化                   */
    pcd_Init();                                                         /* 读卡芯片初始化               */
    //UARTInit();                                                         /* MCU串口初始化                */
    statues = 0;
    while(statues == 0)  {                                              /* 判断SPI接口是否正常通信      */
         statues = MCU_TO_PCD_TEST();            
    }
    if(statues == TRUE){                                                /* 通信正常，红灯亮一下         */
            
		//LED_RedOn();
		printf("FM175xx Demo Test Start!\r\n");
		Delay100us(10000);
		//LED_RedOff();
		
		while (1) {
			//printf("test\n");
		/*	if( (LPC_GPIO_PORT->PIN[0]& CON) == 0 )   {                 // CON接地进入串口解析命令      
				UartCommand();                                          // 串口命令                     
			}
			else  {      */                                               // 通常自动寻卡                 
				TyteA_Test();                                           // ISO14443A自动寻卡测试        
				TyteB_Test();                                           // ISO14443B自动寻卡测试        
			//}
		}
                
    }
	
	return 0;
}






