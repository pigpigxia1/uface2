
extern unsigned char  picc_atqa[2],picc_uid[15],picc_sak[3];

extern unsigned char TypeA_Request(int fd,unsigned char *pTagType);
extern unsigned char TypeA_WakeUp(int fd,unsigned char *pTagType);
extern unsigned char TypeA_Anticollision(int fd,unsigned char selcode,unsigned char *pSnr);
extern unsigned char TypeA_Select(int fd,unsigned char selcode,unsigned char *pSnr,unsigned char *pSak);
extern unsigned char TypeA_Halt(int fd,unsigned char AnticollisionFlag);
extern unsigned char TypeA_CardActive(int fd,unsigned char *pTagType,unsigned char *pSnr,unsigned char *pSak,unsigned char *nbit);
