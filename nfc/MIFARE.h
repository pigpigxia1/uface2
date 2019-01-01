#ifndef _MIFARE_H_
#define _MIFARE_H_
#define KEY_A_AUTH 0
#define KEY_B_AUTH 1

extern unsigned char SECTOR,BLOCK,BLOCK_NUM;
extern unsigned char BLOCK_DATA[16];
extern unsigned char KEY_A[16][6];
extern unsigned char KEY_B[16][6];
extern void Mifare_Clear_Crypto(int fd);
extern unsigned char Mifare_Transfer(int fd,unsigned char block);
extern unsigned char Mifare_Restore(int fd,unsigned char block);
extern unsigned char Mifare_Blockset(int fd,unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Blockinc(int fd,unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Blockdec(int fd,unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Blockwrite(int fd,unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Blockread(int fd,unsigned char block,unsigned char *data_buff);
extern unsigned char Mifare_Auth(int fd,unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid);
#endif
