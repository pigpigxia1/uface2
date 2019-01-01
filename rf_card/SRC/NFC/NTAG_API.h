

#ifndef _NATG_API_H
#define _NTAG_API_H



extern unsigned char Ntag_Block[64][4];                  
void NTAG_Ins_Event(int fd,unsigned char *rBuf,unsigned char rlength);
extern void NTAG_Event(int fd);


#endif


