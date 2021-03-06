#ifndef __CARD_H
#define __CARD_H


#define TYPE_A_M   0x01
#define TYPE_A_NFC 0x02
#define TYPE_A_CPU 0x04
#define TYPE_B_ID  0x08

#define BUFFSIZE 8192  //8k
#define CPU_FILE_LEN 4096
#define EMPT_NUM  8   //预留字节

#define M1_START_SECTOR 0x01
#define M1_FILE_LEN 720
#define ONE_SEC_BYTE 48
unsigned char Check_Machine(int fd);
unsigned char Get_Machine_Type();
void Card_init(int fd,int type);
unsigned char Card_WakeUp(int fd,int type,unsigned char *pTagType,unsigned int *rece_len,unsigned char *buff,unsigned char *card_sn);
unsigned char Card_Halt(int fd,int type,unsigned char AnticollisionFlag,unsigned char *card_sn);
int Get_Card_ID(int fd,unsigned char *Id_buf,int type,int *IC_type);

int card_pthread_init(int fd,unsigned char *key);
unsigned char Write_Card_Info(int fd,unsigned char *old_key,unsigned char *new_key,unsigned char *buff,int len);
int Read_Card(int fd,unsigned char *key,unsigned char *buff,int len);
int Get_Uid(int fd,unsigned char *buff,int len);

#endif