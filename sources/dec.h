#ifndef __DEC_H
#define __DEC_H


void setkey( unsigned char const *userkey );
void enctransform( const unsigned char *In,unsigned char *Out );
void dectransform( const unsigned char *In , unsigned char *Out);









#endif