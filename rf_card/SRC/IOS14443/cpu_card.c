 #include "fm175xx.h"
// #include "des.h"
 #include "cpu_card.h"
 #include "des.h"
 //#include "main.h"
 #include <string.h>
 #include <unistd.h>
 unsigned char  PCB,CID;
  
 #define OK 1

 extern int logfd;
 
/****************************************************************/
/*名称: Rats                                                     */
/*功能: 该函数实现转入APDU命令格式处理    CPU card reset            */
/*输入: param1 byte :PCB                                         */
/*        param2 byte bit8-bit5：FSDI, bit4-bit0 ：CID              */
/*输出:                                                            */
/*        rece_len：接收的数据长度                                */
/*        buff：接收的数据指针                                    */
/* OK: 应答正确                                                    */
/* ERROR: 应答错误                                                */
/****************************************************************/
unsigned char CPU_Rats(int fd,unsigned char param1,unsigned char param2,unsigned char *rece_len,unsigned char *buff)
{
    unsigned char  ret,send_byte[2];
    unsigned int  rece_bitlen;
    Pcd_SetTimer(fd,100);
    send_byte[0] = 0xE0;//Start byte
    send_byte[1] = param2;
    CID = param2 & 0x0f;
    PCB = param1;
    Clear_FIFO(fd);
    ret=Pcd_Comm(fd,Transceive,send_byte,2,buff,&rece_bitlen,0);
    if (ret==OK)
        *rece_len=rece_bitlen/8;
    else
        *rece_len=0;
    return ret;    
}
/****************************************************************/
/*名称: PPS                                                     */
/*功能:                                                         */
/*输入: PPSS byte, PPS0 byte, PPS1 byte                            */
/*输出:                                                            */
/*        rece_len：接收的数据长度                                */
/*        buff：接收的数据指针                                    */
/* OK: 应答正确                                                    */
/* ERROR: 应答错误                                                */
/****************************************************************/
unsigned char CPU_PPS(int fd,unsigned char pps0,unsigned char pps1,unsigned char *rece_len,unsigned char *buff)
{
    unsigned char ret,send_byte[3];
    unsigned int rece_bitlen;
    Pcd_SetTimer(fd,50);
    send_byte[0] = 0xD0|CID;//ppss byte
    send_byte[1] = pps0;    //pps0 byte
    Clear_FIFO(fd);
    if ((pps0&0x10)==0x10)
    {
        send_byte[2]=pps1;     //pps1 byte
        ret = Pcd_Comm(fd,Transceive,send_byte,3,buff,&rece_bitlen ,0);
    }
    else
    {    
        ret = Pcd_Comm(fd,Transceive,send_byte,2,buff,&rece_bitlen ,0);
    }
    if (ret==OK)
        *rece_len=rece_bitlen/8;
    else
        *rece_len=0;
    return ret;    
}

void CPU_PCB(void)
{
//PCB bit0=~(bit0)
    PCB=PCB^0x01;//设置块号变换
return;
}

unsigned char CPU_Des(int fd,unsigned char *key,unsigned char *rece_len,unsigned char *buff)
{
	int i;
	
	unsigned char ret,send_byte[13] = {0x00,0x88,0x00,0x01,0x08};
	
	for(i = 0;i < 8; i++)
	{
		send_byte[i+5] = key[i];
	}
	
	ret=CPU_I_Block(fd,13,send_byte,&(*rece_len),buff);
    return ret; 
}
unsigned char CPU_Slect(int fd,unsigned char *rece_len,unsigned char *buff)
{
	unsigned char ret,send_byte[5] = {0x00,0xA4,0x00,0x00,0x00};
	ret=CPU_I_Block(fd,5,send_byte,&(*rece_len),buff);
    return ret; 
}

unsigned char CPU_Mf_Erase(int fd,unsigned char *rece_len,unsigned char *buff)
{
	unsigned char ret,send_byte[5] = {0x80,0x0E,0x00,0x00,0x00};
	ret=CPU_I_Block(fd,5,send_byte,&(*rece_len),buff);
    return ret; 
}

unsigned char CPU_Key_Create(int fd,unsigned char *rece_len,unsigned char *buff)
{
	unsigned char ret,send_byte[12] = {0x80,0xE0,0x00,0x00,0x07,0x3F,0x00,0x50,0x01,0xF0,0xFF,0xFF};
	ret=CPU_I_Block(fd,12,send_byte,&(*rece_len),buff);
    return ret; 
}

unsigned char CPU_Key_write(int fd,unsigned char *rece_len,unsigned char *buff)
{
	unsigned char ret,send_byte[18] = {0x80,0xD4,0x01,0x00,0x0D,0x36,0xF0,0xF0,0xFF,0x88,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	ret=CPU_I_Block(fd,18,send_byte,&(*rece_len),buff);
    return ret; 
}


/****************************************************************/
/*名称: 随机数                                                    */
/*功能:                                                         */
/*输入: N/A                                                     */
/*输出:                                                            */
/*        rece_len：接收的数据长度                                */
/*        buff：接收的数据指针                                    */
/*         OK: 应答正确                                            */
/*         ERROR: 应答错误                                            */
/****************************************************************/
unsigned char CPU_Random(int fd,unsigned char *rece_len,unsigned char *buff)
{
    unsigned char ret,send_byte[5] = {0x00,0x84,0x00,0x00,0x08};
    //memcpy(send_byte,"\x00\x84\x00\x00\x08",5);
    ret=CPU_I_Block(fd,5,send_byte,&(*rece_len),buff);
    return ret;    
}
/****************************************************************/
/*名称: 外部认证                                                */
/*功能:                                                         */
/*输入: N/A                                                     */
/*输出:                                                            */
/* OK: 应答正确                                                    */
/* ERROR: 应答错误                                                */
/****************************************************************/
unsigned char CPU_auth(int fd,unsigned char *key_buff,unsigned char *rece_len,unsigned char *buff)
{
    unsigned char i,ret,send_byte[15],rece_byte[16],Random_data[8],des_buff[8],temp;
	//unsigned char key[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    ret=CPU_Random(fd,&temp,rece_byte);
    if (temp <= 0)
        return ret;
    for (i=0;i<8;i++)
		Random_data[i]=rece_byte[i+2];
    //for (i=4;i<8;i++)
   // Random_data[i]=0;
    //Tdes(ENCRY,Random_data,key_buff,des_buff);
	des_ecb_encrypt(des_buff,Random_data,8,key_buff);
    //CPU_PCB();
    //send_byte[0] = PCB;
    //send_byte[1] = CID;
    send_byte[0] = 0x00;
    send_byte[1] = 0x82;
    send_byte[2] = 0x00;
    send_byte[3] = 0x00;
    send_byte[4] = 0x08;
    for(i=0;i<8;i++)
    send_byte[i+5]= des_buff[i];
    ret=CPU_I_Block(fd,13,send_byte,&(*rece_len),buff);
    return ret;    
    
}
/****************************************************************/
/*名称: CPU_command     cpu card operation                        */
/*功能: CPU命令操作                                                */
/*输入:                                                            */
/*                                                                */
/*        send_len:发送的指令长度                                 */
/*        send_buff:发送的指令数据指针                             */
/*                                                                */
/*输出:响应长度                                                    */
/*        rece_len：接收的数据长度                                */
/*        rece_buff：接收的数据指针                                */
/*         OK: 应答正确                                            */
/*         ERROR: 应答错误                                            */
/****************************************************************/
unsigned char CPU_TPDU(int fd,unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff)
{    unsigned char ret;
    unsigned int rece_bitlen;

    Pcd_SetTimer(fd,100);
    ret=Pcd_Comm(fd,Transceive,send_buff,send_len,rece_buff,&rece_bitlen,0);
    if (ret==OK)
            *rece_len=rece_bitlen/8;
        else
            *rece_len=0;
    return ret;
}
/****************************************************************/
/*名称: I Block     cpu card operation                            */
/*功能: CPU命令操作(I block)                                    */
/*输入:                                                            */
/*                                                                */
/*        send_len:发送的指令长度                                 */
/*        send_buff:发送的指令数据指针                             */
/*                                                                */
/*输出:响应长度                                                    */
/*        rece_len：接收的数据长度                                */
/*        rece_buff：接收的数据指针                                */
/*         OK: 应答正确                                            */
/*         ERROR: 应答错误                                            */
/****************************************************************/
 unsigned char CPU_I_Block(int fd,unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff)
 {
        unsigned char ret;
        unsigned int rece_bitlen;
		int i;
		unsigned char buf[64];
        Pcd_SetTimer(fd,500);
        Clear_FIFO(fd);
        /* if (PCB&0x08)//判断是否有CID字节
        {
            Write_FIFO(fd,1,&PCB);
            Write_FIFO(fd,1,&CID);
        }
        else
        {
            Write_FIFO(fd,1,&PCB);
        }*/
		if (PCB&0x08)//判断是否有CID字节
        {
            buf[0] = PCB;
			buf[1] = CID;
			for(i = 0; i < send_len;i++)
				buf[i + 2] = send_buff[i];
        }
        else
        {
            
        }
		
		write(logfd,buf,send_len + 2);
		//write(logfd,"",3);
        ret=Pcd_Comm(fd,Transceive,buf,send_len + 2,rece_buff,&rece_bitlen,0);
        if (ret==OK)
        {
            PCB=*rece_buff;
            CPU_PCB();
            *rece_len=rece_bitlen/(unsigned int)8;
        }
        else
            *rece_len=0;
        return ret;
 }
/****************************************************************/
/*名称: R Block     cpu card operation                            */
/*功能: CPU命令操作(R block)                                    */
/*输入:                                                            */
/*                                                                */
/*                                                                */
/*输出:响应长度                                                    */
/*        rece_len：接收的数据长度                                */
/*        rece_buff：接收的数据指针                                */
/*         OK: 应答正确                                            */
/*         ERROR: 应答错误                                            */
/****************************************************************/

 unsigned char CPU_R_Block(int fd,unsigned char *rece_len,unsigned char *rece_buff)
 {
     unsigned char send_buff[2],ret;
    unsigned int rece_bitlen;
    Pcd_SetTimer(fd,100);
    Clear_FIFO(fd);
    if (PCB&0x08)//判断是否有CID字节
        {
          send_buff[0] = (PCB&0x0F)|0xA0;
        send_buff[1] = CID;
        ret=Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
        }
    else
        {
        send_buff[0] = (PCB&0x0F)|0xA0;
        ret=Pcd_Comm(fd,Transceive,send_buff,1,rece_buff,&rece_bitlen,0);
        }
    if (ret==OK)
        {
            PCB=*rece_buff;
            CPU_PCB();
            *rece_len=rece_bitlen/8;
        }
    else
            *rece_len=0;
    return ret;
 }
/****************************************************************/
/*名称: S Block     cpu card operation                            */
/*功能: CPU命令操作(S block)                                    */
/*输入:                                                            */
/*                                                                */
/*        pcb_byte:设置PCB字节                                    */
/*                                                                */
/*输出:响应长度                                                    */
/*        rece_len：接收的数据长度                                */
/*        rece_buff：接收的数据指针                                */
/*         OK: 应答正确                                            */
/*         ERROR: 应答错误                                            */
/****************************************************************/
 unsigned char CPU_S_Block(int fd,unsigned char pcb_byte,unsigned char *rece_len,unsigned char *rece_buff)
 {
   unsigned char  send_buff[3],ret;
   unsigned int  rece_bitlen;
   Pcd_SetTimer(fd,500);
   Clear_FIFO(fd);
   send_buff[0] =0xC0|pcb_byte;

   if ((PCB&0x08)==0x08)//判断是否有CID字节
   {
       if ((pcb_byte&0xF0)==0xF0) //WTX
       {
              send_buff[1] = CID;
            send_buff[2] = 0x01;
            ret=Pcd_Comm(fd,Transceive,send_buff,3,rece_buff,&rece_bitlen,0);
        }
        else  //DESELECT
        {
            send_buff[1] = CID;
            ret=Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
        }
    }
    else
    {
        if ((pcb_byte&0xF0)==0xF0) //WTX
       {
              send_buff[1] = 0x01;
            ret=Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
        }
        else   //DESELECT
        {
            send_buff[1] = 0x00;
            ret=Pcd_Comm(fd,Transceive,send_buff,2,rece_buff,&rece_bitlen,0);
        }
    }
    if (ret==OK)
        {
            PCB=*rece_buff;
            CPU_PCB();
            *rece_len=rece_bitlen/8;
        }
    else
            *rece_len=0;
    return ret;
 }

/****************************************************************/
/*名称: CPU_baud_rate_change                                    */
/*功能: 调整通信波特率                                            */
/*输入:                                                            */
/*        mode=0 接收发送波特率106kbps                            */
/*        mode=1 接收发送波特率212kbps                            */
/*        mode=2 接收发送波特率424kbps                             */
/*                                                                */
/*输出:响应长度                                                    */
/*         OK: 应答正确                                            */
/*         ERROR: 应答错误                                            */
/****************************************************************/
unsigned char CPU_BaudRate_Set(int fd,unsigned char mode) //mode=0:106Kbps;1:212Kpbs;2:424Kpbs
{
    Clear_BitMask(fd,TxModeReg,0x70);
    Clear_BitMask(fd,RxModeReg,0x70);
  switch (mode)
  {
  case 0:      //106kbps
         Set_BitMask(fd,TxModeReg,0x00);
        Set_BitMask(fd,RxModeReg,0x00);
        break;
  case 1:      //212kbps
        Set_BitMask(fd,TxModeReg,0x10);
        Set_BitMask(fd,RxModeReg,0x10);
        break;
  case 2:      //424kbps
        Set_BitMask(fd,TxModeReg,0x20);
        Set_BitMask(fd,RxModeReg,0x20);
        break;
  case 3:      //848kbps
        Set_BitMask(fd,TxModeReg,0x30);
        Set_BitMask(fd,RxModeReg,0x30);
        break;
  }
return OK;
}

