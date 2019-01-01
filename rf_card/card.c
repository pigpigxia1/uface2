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
** File name:           test.c
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
#include <unistd.h>

static unsigned char type = 0;

int check_type(int fd)
{
	if(type == 0)
	{
		pcd_RST();
		Read_Reg(fd,ControlReg);
		Write_Reg(fd,ControlReg, 0x10);
		if(Read_Reg(fd,ControlReg) == 0x10)
			type = 1;
		else 
		{
			type = 2;
		}
		printf("card type =%d\n",type);
	}
	
	
	return type;
}

int get_card_id(int fd,unsigned char *id)
{
	int n;
	int i;
	unsigned char statues = TRUE;
	
    unsigned char num=0;
	unsigned char nbit = 0;
	uint32_t rec_len;
    unsigned char pupi[4];
	unsigned char read_buf[512];
    unsigned char picc_atqa[2];                           /* 储存寻卡返回卡片类型信息 */
    unsigned char picc_sak[3];                            /* 储存卡片应答信息*/  
	
	check_type(fd);
	if(type != 0)
	{
		if(type == 1)
		{
			//typeA
			FM175X_SoftReset( fd);             /* FM175xx软件复位        */
			Set_Rf(fd, 3 );                   /* 打开双天线              */
			Pcd_ConfigISOType(fd, 0 );       /* ISO14443寄存器初始化     */
			
			while(num <2 ) {
				statues = TypeA_CardActive(fd, picc_atqa,id,picc_sak ,&nbit);      /* 激活卡片    */
				if ( statues == TRUE ) {
					num = 0;
					TypeA_Halt(fd,0);       /* 睡眠卡片   */
					goto end;
					
					//printf("CardUID:0x");
					//UartSendUid(picc_atqa,id);    
					printf("\r\nTyteA_Test\n");     
					//return nbit;
					//memset(id,0x00,15);                    
				}
				else {
					nbit = 0;
					num++;
				}                    
			}
			//Delay100us(1000);
			//typeB
			FM175X_SoftReset( fd);                                      /* FM175xx软件复位       */
			Pcd_ConfigISOType(fd, 1 );                              /* ISO14443寄存器初始化      */
			Set_Rf( fd,3 );                                         /* 打开双天线                 */ 
			statues = TypeB_WUP(fd,&rec_len,id,pupi);              /* 寻卡                      */
			if ( statues == TRUE ) {
				statues = TypeB_Select(fd,pupi,&rec_len,id); 
			}
			if ( statues == TRUE ) {
				//LED_RedOn();
				nbit = rec_len;
				statues = TypeB_GetUID(fd,&rec_len,&id[0]);
				if(statues == TRUE) {
					//printf("CardUID:0x");                                  /* 输出UID号码   */
					//uartSendHex( id, 8 );
					printf("\r\nTyteB_Test\n");
					//return nbit;
					goto end;
				}
				else
				{
					nbit = 0;
				}
				
			}
			Set_Rf(fd, 0 );                                                /* 关闭天线   */
		}
		else if(type == 2 )
		{
			if(n = read(fd,read_buf,512) > 0)
			{
				if((read_buf[0] == 0x04)&&(n > 4))
				{
					memcpy(id,read_buf,4);
					nbit = 4;
				}
				else if((read_buf[0] == 0x08)&&(n > 8))
				{
					memcpy(id,read_buf,8);
					nbit = 8;
				}
				else
				{
				/*	for(i = 0;i < n;i++)
						printf("%x\t",read_buf[i]);
					printf("\n");*/
					memset(read_buf,0,512);
					nbit = 0;
				}
					
			}
			
		}
	}
	else
	{
		printf("please check type!\n");
		return -1;
	}

end:
	return nbit;
	
}

unsigned char get_version(int fd)
{
	return Read_Reg(fd,VersionReg);
}




