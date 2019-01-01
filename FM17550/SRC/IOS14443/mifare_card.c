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
** File name:           mifare_card.c
** Last modified Date:  2016-3-14
** Last Version:        V1.00
** Descriptions:        mifare系列卡相关操作
**
**--------------------------------------------------------------------------------------------------------
*/
#include "mifare_card.h"
#include "fm175xx.h"
#include "Utility.h"

/*********************************************************************************************************
** Function name:       Mifare_Auth        
** Descriptions:        mifare卡片校验
** input parameters:    mode，认证模式（0：key A认证，1：key B认证）
**                      sector，认证的扇区号（0~15）
**                      *mifare_key，6字节认证密钥数组
**                      card_uid 4字节卡片UID数组        
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
 unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
    unsigned char  send_buff[12],rece_buff[1],result;
    unsigned int  rece_bitlen;
    if(mode==0x0)
        send_buff[0]=0x60;                                              /* kayA认证 0x60                */
    if(mode==0x1)
        send_buff[0]=0x61;                                              /* keyB认证                     */
    send_buff[1] = sector*4;
    send_buff[2] = mifare_key[0];
    send_buff[3] = mifare_key[1];
    send_buff[4] = mifare_key[2];
    send_buff[5] = mifare_key[3];
    send_buff[6] = mifare_key[4];
    send_buff[7] = mifare_key[5];
    send_buff[8] = card_uid[0];
    send_buff[9] = card_uid[1];
    send_buff[10] = card_uid[2];
    send_buff[11] = card_uid[3];

    Pcd_SetTimer(1);
    Clear_FIFO();
    result =Pcd_Comm(MFAuthent,send_buff,12,rece_buff,&rece_bitlen);    /* Authent认证                  */
    if (result==TRUE) {
        if(Read_Reg(Status2Reg)&0x08)                                   /* 判断加密标志位，确认认证结果 */
            return TRUE;
        else
            return FALSE;
    }
    return FALSE;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockset        
** Descriptions:        mifare卡设置卡片计数值
** input parameters:    block:块号
**                      buff:4字节初始值
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockset(unsigned char block,unsigned char *buff)
{
    unsigned char  block_data[16],result;
    block_data[0]=buff[3];
    block_data[1]=buff[2];
    block_data[2]=buff[1];
    block_data[3]=buff[0];
    block_data[4]=~buff[3];
    block_data[5]=~buff[2];
    block_data[6]=~buff[1];
    block_data[7]=~buff[0];
    block_data[8]=buff[3];
    block_data[9]=buff[2];
    block_data[10]=buff[1];
    block_data[11]=buff[0];
    block_data[12]=block;
    block_data[13]=~block;
    block_data[14]=block;
    block_data[15]=~block;
    result= Mifare_Blockwrite(block,block_data);
    return result;
}

/*********************************************************************************************************
** Function name:       Mifare_Blockread        
** Descriptions:        mifare卡读取块函数
** input parameters:    block:块号   0x00-0x3f       
** output parameters:   buff:读出16字节数据 
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *buff)
{    
    unsigned char  send_buff[2],result;
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x30;                                                  /* 读操作命令                   */
    send_buff[1]=block;                                                 /* 读操作地址                   */
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//
    if ((result!=TRUE )|(rece_bitlen!=16*8))                            /* 接收到的数据长度为16         */
        return FALSE;
    return TRUE;
}

/*********************************************************************************************************
** Function name:       mifare_blockwrite        
** Descriptions:        mifare卡写块函数
** input parameters:    block:块号   0x00-0x3f    
**                      buff:读出16字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[16],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0xa0;                                                  /* 写块命令                     */
    send_buff[1]=block;                                                 /* 块地址                       */

    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return(FALSE);

    Pcd_SetTimer(5);
    Clear_FIFO();
    result =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    return TRUE;
}


/*********************************************************************************************************
** Function name:       Mifare_Blockinc        
** Descriptions:        mifare卡片增值操作    
** input parameters:    block:块号   0x00-0x3f    
**                      buff:增加4字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockinc(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc1;                                                  /* 增值操作命令                 */
    send_buff[1]=block;                                                 /* 块地址                       */
    Clear_FIFO();
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    Pcd_SetTimer(5);
    Clear_FIFO();
    Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Blockdec        
** Descriptions:        mifare卡片减值操作    
** input parameters:    block:块号   0x00-0x3f    
**                      buff:减少4字节数据 
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Blockdec(unsigned char block,unsigned char *buff)
{    
    unsigned char  result,send_buff[2],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc0;
    send_buff[1]=block;                                                 /* 块地址                       */
    Clear_FIFO();
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result!=TRUE )|((rece_buff[0]&0x0F)!=0x0A))                    /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    Pcd_SetTimer(5);
    Clear_FIFO();
    Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Transfer        
** Descriptions:        mifare卡片传输块
** input parameters:    block:块号   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Transfer(unsigned char block)
{    
    unsigned char   result,send_buff[2],rece_buff[1];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0] = 0xb0;
    send_buff[1] = block;                                               /* 块地址                       */
    Clear_FIFO();
    result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    return result;
}


/*********************************************************************************************************
** Function name:       Mifare_Restore        
** Descriptions:        mifare卡片储存命令
** input parameters:    block:块号   0x00-0x3f    
** output parameters:   
** Returned value:      TRUE：操作成功 ERROR：操作失败    
*********************************************************************************************************/
unsigned char Mifare_Restore(unsigned char block)
{    
    unsigned char result,send_buff[4],rece_buff[1];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc2;
    send_buff[1]=block;                                                 /* 块地址                       */
    Clear_FIFO();
    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    if ((result != TRUE )|((rece_buff[0]&0x0F) != 0x0A))                /* 如果未接收到0x0A，表示无ACK  */
        return FALSE;
    Pcd_SetTimer(5);
    send_buff[0]=0x00;
    send_buff[1]=0x00;
    send_buff[2]=0x00;
    send_buff[3]=0x00;
    Clear_FIFO();
    Pcd_Comm(Transceive,send_buff,4,rece_buff,&rece_bitlen);
    return result;
}
