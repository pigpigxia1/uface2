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
#include "card.h"
#include <time.h>
#include <sys/time.h>

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
//extern unsigned char get_version(int fd);


/*
static void Print(unsigned char *buff,unsigned char len)
{
	unsigned char i;
	printf("\nlen =%x\n",len);
	for(i = 0;i < len;i++)
	{
		printf("%x\t",buff[i]);
	}
	printf("\n");
}
*/

const unsigned char new_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//unsigned char default_key[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char M1_buff[500] = {0x00,0x20,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xfe,0xff};
unsigned char CPU_buff[1024] = {0x00,0x64,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x00};

int main(void)
{
#if 1
	int fd = -1;
	int i,n;
	int cmd;
	//clock_t t1,t2;
	struct timeval start,end;
	int len;
	int info_len;
	int nbit = 0;
	unsigned char machine_type;
	unsigned char reg;
	
	unsigned char dat_buf[2048] = {0x0};
	unsigned char *Pbuf = dat_buf;
	
    
    Delay100us(1000);
                   
    pcd_Init();                                                         // 读卡芯片初始化               
    
	
	fd = uart_init(9600,8,'N',1);
	
	
	//cpu_test(fd);
	//machine_type = Check_Machine(fd);
	//printf("machine_type = %d\n",machine_type);
	
	card_pthread_init(fd,new_key);
	//SetSpeed(fd,115200);
	
	/*printf("reg=%x\n",Read_Reg(fd,SerialSpeedReg));
	
	printf("enter card type:(1:M1 2:cpu)\n");
	scanf("%d",&cmd);
	
	if(cmd == 1)
	{
		n = M1_buff[0]*256 + M1_buff[1] - 1;
		for(i = 0;i < n;i++)
			M1_buff[i+2] = i;
		M1_buff[M1_buff[0]*256 + M1_buff[1] +1] = GetByteBCC(&M1_buff[2],M1_buff[0]*256 + M1_buff[1]-1);
		Pbuf = M1_buff;
	}
		
	else if(cmd == 2)
	{
		n = CPU_buff[0]*256 + CPU_buff[1] - 1;
		for(i = 0;i < n;i++)
			CPU_buff[i+2] = i;
		CPU_buff[CPU_buff[0]*256 + CPU_buff[1] +1] = GetByteBCC(&CPU_buff[2],CPU_buff[0]*256 + CPU_buff[1]-1);
		Pbuf = CPU_buff;
	}
	
	Write_Card_Info(fd,default_key,new_key,Pbuf);*/
	while(1)
	{
		gettimeofday(&start, NULL);
		printf("main run\n");
		len = Read_Info(Pbuf,15);
		if(len > 0)
		{
			Print(Pbuf,len);
		
			if(Pbuf[13]!=0xcc && Pbuf[14]!=0xdd)
			{
				Pbuf += 15;
				do{
					len = Read_Info(Pbuf,2);
					if(len == 2)
					{
						printf("%s:%d info_len=%d\n",__FUNCTION__,__LINE__,info_len);
						Print(Pbuf,len);
						info_len = Pbuf[0] * 256 + Pbuf[1] + 2;
						printf("%s:%d info_len=%d\n",__FUNCTION__,__LINE__,info_len);
						nbit = info_len;
				//do{
						Pbuf += len;
						len = Read_Info(Pbuf,nbit);
						Print(Pbuf,len);
						nbit -= len;
				//}while(nbit > 0);
					}
					
				}while(len == 0);
				
				
			}
			gettimeofday(&end, NULL);
			printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
		}
		/*Pbuf += 3;
		if(len == 3&& Pbuf[0] == 0xaa && Pbuf[1] == 0xbb)
		{
			len = Read_Info(Pbuf,Pbuf[2]+10);
			if(len == Pbuf[2]+10)
			{
				info_len = Pbuf[Pbuf[2] + 8]*256 + Pbuf[Pbuf[2] + 9];
				nbit = info_len;
				do{
					Pbuf += len;
					len = Read_Info(Pbuf,nbit);
					nbit -= len;
				}while(nbit > 0);
				
			}
			Print(dat_buf,info_len + dat_buf[2] + 13);
		}*/
		printf("%s:%d\n",__FUNCTION__,__LINE__);
		
		/*if(len > dat_buf[2] + 1 + EMPT_NUM)
		{
			printf("%s:%d\n",__FUNCTION__,__LINE__);
			if(dat_buf[dat_buf[0] + 1 + EMPT_NUM] == 0xAA && dat_buf[dat_buf[0] + 1 + EMPT_NUM + 1] == 0xBB)
			{
				Print(dat_buf,dat_buf[0] + 1 + EMPT_NUM);
			}
			else
			{
				info_len = dat_buf[dat_buf[0] + 1 + EMPT_NUM] * 256 + dat_buf[dat_buf[0] + 1 + EMPT_NUM + 1];
				nbit = info_len + EMPT_NUM + dat_buf[0] + 1 - len;
				do{
					Pbuf += len;
					printf("%s:%d len=%d nbit=%d\n",__FUNCTION__,__LINE__,len,nbit);
					len = Read_Info(Pbuf,nbit);
					nbit -= len;
				}while(nbit > 0);
				Print(dat_buf,info_len + EMPT_NUM + dat_buf[0] + 1);
			}
			
		}*/
		//TyteA_Test(fd);
		//TyteB_Test(fd);
		//t1 = clock();
		/*gettimeofday(&start, NULL);
		len = Read_Card(fd,new_key,dat_buf);
		gettimeofday(&end, NULL);
		//t2 = clock();
		printf("total time = %dms\n",(end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec)/1000);
		if(len  > 0)
			Print(dat_buf,len);*/
		
		usleep(100000);
			
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






