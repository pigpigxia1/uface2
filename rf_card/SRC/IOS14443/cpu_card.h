#ifndef CPU_CARD_H
#define CPU_CARD_H 1
//struct apdu_frame
//{
//unsigned char pcb;
//unsigned char cid;
//unsigned char buff[62];
//};

extern unsigned char  PCB,CID;

extern void CPU_PCB(void);
extern unsigned char CPU_BaudRate_Set(int fd,unsigned char mode);
extern unsigned char CPU_TPDU(int fd,unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_Rats(int fd,unsigned char param1,unsigned char param2,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_PPS(int fd,unsigned char pps0,unsigned char pps1,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Mf_Erase(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Slect(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Key_Create(int fd,unsigned char *rece_len,unsigned char *buff);
unsigned char CPU_Key_write(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_Random(int fd,unsigned char *rece_len,unsigned char *buff);
extern unsigned char CPU_I_Block(int fd,unsigned char send_len,unsigned char *send_buff,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_R_Block(int fd,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_S_Block(int fd,unsigned char pcb_byte,unsigned char *rece_len,unsigned char *rece_buff);
extern unsigned char CPU_auth(int fd,unsigned char *key_buff,unsigned char *rece_len,unsigned char *buff);
#endif
