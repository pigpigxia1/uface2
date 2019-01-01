

#include <stdio.h>
#include "FM175XX_REG.h"
#include "FM175XX.h"
#include "READER_API.h"
#include "MIFARE.h"
//#include "READER_API.h"
#include "CARD_EMULATE_API.h"
#include "NTAG_API.h"
#include "uart.h"


void TYPE_A_EVENT(int fd);
void TYPE_B_EVENT(int fd);


static void uartSendByte(unsigned char value)
{
	printf("%c",value);
}

static void uartSendHex (unsigned char *pucStr,unsigned char num)
{
    unsigned char hChar,lChar;
    unsigned char *pChar;
    pChar = pucStr;
    while(num--)                 
    {
        hChar = *(pChar)>>4;
        lChar = *(pChar++) & 0x0f;
        if( hChar < 0x0a )  {
            hChar += '0';
        }
        else {
            hChar = hChar-10+'A';
        }
        if( lChar < 0x0a )  {
            lChar += '0';
        }
        else {
            lChar = lChar-10+'A';
        }
        uartSendByte(hChar);
        uartSendByte(lChar);
    }
}


int main(void)
{

	int fd;
	
	FM175XX_HardReset();
	//printf("->NTAG Start!\r\n");

	pcd_Init();
	fd = uart_init(9600,8,'N',1);
#if 0
	Card_Emulate_Init(fd);	
	Card_Emulate_Config(fd,CARD_UID_INFO);

	while(1)
	{					
		if (CARD.IRQ == TRUE)
		{							
			Card_Emulate_IRQ_Event(fd,JBIT_HIALERTI | JBIT_RXI | JBIT_ERRI,0);				
			NTAG_Event(fd);										
		}	
		
		else  
		{
			Card_Emulate_IRQ_Event(fd,0,JBIT_RFONI|JBIT_RFOFFI);
		}
	
	}
#else
	unsigned char polling_card;

	Set_EMV_Enhance(fd,1);

	while(1)
	{
		sleep(1);	
		printf("run\n");
		if(FM175XX_Polling(fd,&polling_card)== SUCCESS)
		{
			printf("FM175XX_Polling");
			if(polling_card & BIT0)//TYPE A
			{	
				TYPE_A_EVENT(fd);							
			}

			if(polling_card & BIT1)
			{							
				TYPE_B_EVENT(fd);
			}
		}					
	
	}
#endif
		
}



void TYPE_A_EVENT(int fd)
{
		unsigned char result;
		printf("\r\n-> TYPE A CARD!\r\n");	
		
		FM175XX_Initial_ReaderA(fd);
		SetRf(fd,3);		
		result = ReaderA_CardActivate(fd);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}
		
		printf("-> ATQA = ");
		uartSendHex(Type_A.ATQA, 2);
		printf("\r\n");		
		
		printf("-> UID = ");
		uartSendHex(Type_A.UID, 4 * Type_A.CASCADE_LEVEL);
		printf("\r\n");		
		
		printf("-> SAK = ");
		uartSendHex(Type_A.SAK, Type_A.CASCADE_LEVEL);
		printf("\r\n");	
						
		SECTOR = 0;
		BLOCK = 1;
		BLOCK_NUM = (SECTOR * 4) + BLOCK;  
		result = Mifare_Auth(fd,KEY_A_AUTH,SECTOR,KEY_A[SECTOR],Type_A.UID);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}
						
		printf("-> AUTH SUCCESS!\r\n");			
	
				
		result = Mifare_Blockwrite(fd,BLOCK,BLOCK_DATA);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}
		printf("-> WRITE BLOCK SUCCESS!\r\n");
				
		result = Mifare_Blockread(fd,BLOCK,BLOCK_DATA);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}
		printf("-> READ BLOCK = ");
		uartSendHex(BLOCK_DATA,16); 
		printf("\r\n");					
				
		SetRf(fd,0);	
		return;
}

void TYPE_B_EVENT(int fd)
{
		unsigned char result;	
		printf("-> TYPE B CARD!\r\n");			
		FM175XX_Initial_ReaderB(fd);
		SetRf(fd,3);					
		result = ReaderB_Wakeup(fd);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}		
		printf("-> ATQB = ");
		uartSendHex(Type_B.ATQB, 12);
		printf("\r\n");		
					
		result = ReaderB_Attrib(fd);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}
		printf("-> ATTRIB = ");
		uartSendHex(Type_B.ATTRIB, Type_B.LEN_ATTRIB);
		printf("\r\n");		
			
		result = ReaderB_GetUID(fd);
		if(result != FM175XX_SUCCESS)
		{
			SetRf(fd,0);		
			return;
		}
		printf("-> UID = ");
		uartSendHex(Type_B.UID, 8);
		printf("\r\n");			
				
		SetRf(fd,0);	
		return;
}








