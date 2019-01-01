/******************** (C) COPYRIGHT 2014 bjain@ BLZX***********************************
 * File name   : CRC.c
 * Description : CRC 效验处理
 * Version     : 2.0.1
 * Author      : bjian
 * date        : 2014-8-11
 *************************************************************************************/
 
 #include "CRC.h"

int CRC16( char *p, int len )
{
    unsigned char l, h, t;
    int i;
    l=h=0xff;
    for( i=0; i<len; i++ )
    {
        h^=p[i];
        h^=h>>4;
        t=h;
        h=l;
        l=t;
        t=(l<<4) | (l>>4);
        h^=((t<<2) | (t>>6)) & 0x1f;
        h^=t&0xf0;
        l^=((t<<1) | (t>>7)) & 0xe0;
    }
    i = ((int)h<<8) | l;
    return i;
}


