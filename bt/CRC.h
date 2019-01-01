/******************** (C) COPYRIGHT 2014 bjain@ BLZX***********************************
 * File name   : CRC.c
 * Description : CRC 效验处理
 * Version     : 2.0.1
 * Author      : bjian
 * date        : 2014-8-11
 *************************************************************************************/

#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>
#include <string.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t


int CRC16( char *p, int len );



#endif
