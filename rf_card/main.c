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
#include "cpu_card.h"
#include "Utility.h"
#include "mifare_card.h"
#include "des.h"

#include "string.h"

#include <stdio.h>



/*
** 测试函数
*/
extern void cpu_test(int fd);
extern void CPU_Test(int fd);
extern void TyteA_Test(int fd);
extern void TyteB_Test(int fd);
extern void MifareCard_Test (int fd,uint8_t command);
extern int get_card_id(int fd,unsigned char *id);
int get_uid(int fd,unsigned char *id);
extern unsigned char get_version(int fd);

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

int get_id(int fd,unsigned char *uid,int mode)
{
	int ret;
	if(mode)
	{
		ret = get_uid(fd,uid);
	}
	else
	{
		ret = get_card_id(fd,uid);
	}
	return ret;
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
#if 1
	int fd = -1;
	int i;
	int card_type = -1;
	unsigned char uid[10];
	unsigned char nbit;
	volatile uint8_t ucRegVal;
    volatile unsigned char statues;
    
    Delay100us(1000);
                   
    pcd_Init();                                                         // 读卡芯片初始化               
               
	fd = uart_init(9600,8,'N',1);
	
	
    //cpu_test(fd);
		
	while (1) {
		/*nbit = get_card_id(fd,uid);
		//printf("nbit=%d\n",nbit);
		if(nbit > 0)
		{
			printf("\nUID:");
			for(i = 0;i < nbit;i++)
			{
				printf("%x\t",uid[i]);
			}
			
		}
		sleep(2);
		CPU_Test(fd);    */     
		TyteA_Test(fd);
		//TyteB_Test(fd);
		//sleep(1);
			
			
	}
#else
		
	int ret,len,i;
	unsigned char buff[1024] = {0};
	unsigned char dat[16] = {0x87,0x87,0x87,0x87,0x87,0xf87,0x87,0x87};
	unsigned char key[16] = {0x0E,0x32,0x92,0x32,0xEA,0x6D,0x0D,0x73};
	
	
	len = strlen((char*)dat);
        for(i = 0;i < len;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        
        //DES ECB 加密
        printf("DES ECB ENC::\r\n");
        ret = des_ecb_encrypt(buff,dat,len,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //DES ECB 解密
        printf("DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_ecb_decrypt(dat,buff,ret,key);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //DES CBC 加密
        printf("DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des_cbc_encrypt(buff,dat,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //DES CBC 解密
        printf("DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des_cbc_decrypt(dat,buff,ret,key,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
        printf("\r\n");


        //3DES ECB 加密
        printf("3DES ECB ENC::\r\n");
        ret = des3_ecb_encrypt(buff,dat,len,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");
        //3DES ECB 解密
        printf("3DES ECB DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_ecb_decrypt(dat,buff,ret,key,16);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");

        //3DES CBC 加密
        printf("3DES CBC ENC::\r\n");
        memset(buff,0,sizeof(buff));
        des3_cbc_encrypt(buff,dat,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",buff[i]);
        }
        printf("\r\n");

        //3DES CBC 解密
        printf("3DES CBC DEC::\r\n");
        memset(dat,0,sizeof(dat));
        des3_cbc_decrypt(dat,buff,ret,key,16,NULL);
        for(i = 0;i < ret;i++)
        {
            printf("%02X",dat[i]);
        }
        printf("\r\n");
		
#endif
	
                
    
	
	return 0;
}






