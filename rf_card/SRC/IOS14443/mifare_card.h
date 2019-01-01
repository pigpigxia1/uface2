
extern unsigned char Mifare_Transfer(int fd,unsigned char block);
extern unsigned char Mifare_Restore(int fd,unsigned char block);
extern unsigned char Mifare_Blockset(int fd,unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockinc(int fd,unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockdec(int fd,unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockwrite(int fd,unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockread(int fd,unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Auth(int fd,unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid);
