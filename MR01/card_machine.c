#include <stdio.h>
#include <string.h>
#include  <pthread.h>
#include "card_machine.h"
#include "card.h"
#include "uart.h"

const unsigned char cmd_array[][3]=
{
	{SEND_CMD_HEAD,RESET_CMD,RESET_MOVE_TO_OUT},
	{SEND_CMD_HEAD,RESET_CMD,RESET_MOVE_TO_BOX},
	{SEND_CMD_HEAD,RESET_CMD,RESET_NO_MOVE},
	{SEND_CMD_HEAD,RESET_CMD,RESET_MOVE_TO_OUT_C},
	{SEND_CMD_HEAD,RESET_CMD,RESET_MOVE_TO_BOX_C},
	{SEND_CMD_HEAD,RESET_CMD,RESET_NO_MOVE_C},
	
	{SEND_CMD_HEAD,GET_STATUS_CMD,GET_STATUS_BASE},
	{SEND_CMD_HEAD,GET_STATUS_CMD,GET_STATUS_SENSOR},
	
	{SEND_CMD_HEAD,MOVE_CARD_CMD,MOVE_CARD_TO_OUT_HOLD},
	{SEND_CMD_HEAD,MOVE_CARD_CMD,MOVE_CARD_TO_IC_POS},
	{SEND_CMD_HEAD,MOVE_CARD_CMD,MOVE_CARD_TO_RF_POS},
	{SEND_CMD_HEAD,MOVE_CARD_CMD,MOVE_CARD_TO_BOX},
	{SEND_CMD_HEAD,MOVE_CARD_CMD,MOVE_CARD_TO_OUT_NHOLD},
	{SEND_CMD_HEAD,SET_ENTER_CARD_CMD,ENTER_CARD_OK},
	{SEND_CMD_HEAD,SET_ENTER_CARD_CMD,ENTER_CARD_NO},
	{SEND_CMD_HEAD,CARD_TYPE_CMD,CHECK_IC_TYPE},
	{SEND_CMD_HEAD,CARD_TYPE_CMD,CHECK_RF_TYPE},
	
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_RESET},
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_POWER_DOWN},
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_CHECK_STATUS},
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_APDU_0},
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_APDU_1},
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_HOT_RESET},
	{SEND_CMD_HEAD,CPU_CARD_OPERATE_CMD,CPU_CARD_AUTO_TELL_APDU},
	
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_RESET},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_POWER_DOWN},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_CHECK_STATUS},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_APDU_0},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_APDU_1},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_HOT_RESET},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_AUTO_TELL_APDU},
	{SEND_CMD_HEAD,ASM_CARD_OPERATE_CMD,ASM_CARD_SELECT_POS},
	
	{SEND_CMD_HEAD,SEL_CARD_OPERATE_CMD,SEL_CARD_RESET},
	{SEND_CMD_HEAD,SEL_CARD_OPERATE_CMD,SEL_CARD_POWER_DOWN},
	{SEND_CMD_HEAD,SEL_CARD_OPERATE_CMD,SEL_CARD_CHECK_STATUS},
	{SEND_CMD_HEAD,SEL_CARD_OPERATE_CMD,SEL_CARD_4442},
	{SEND_CMD_HEAD,SEL_CARD_OPERATE_CMD,SEL_CARD_4428},
	
	{SEND_CMD_HEAD,I2C_CARD_OPERATE_CMD,I2C_CARD_RESET},
	{SEND_CMD_HEAD,I2C_CARD_OPERATE_CMD,I2C_CARD_POWER_DOWN},
	{SEND_CMD_HEAD,I2C_CARD_OPERATE_CMD,I2C_CARD_CHECK_STATUS},
	{SEND_CMD_HEAD,I2C_CARD_OPERATE_CMD,I2C_CARD_READ},
	{SEND_CMD_HEAD,I2C_CARD_OPERATE_CMD,I2C_CARD_WRITE},
	
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CARD_ACTIV},
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CARD_POWER_DOWN},
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CARD_CHECK_STATUS},
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CARD_MAFARE_RDWR},
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CARD_A_CL_DATA_SWAP},
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CARD_B_CL_DATA_SWAP},
	{SEND_CMD_HEAD,RF_CARD_OPERATE_CMD,RF_CAED_WAKE_SLEEP},
	
	{SEND_CMD_HEAD,CARD_MACHINE_ID_RD_CMD,CAED_MACHINE_ID_RD_PM},
	{SEND_CMD_HEAD,MACHINE_CONF_INFO_CMD,MACHINE_CONF_INFO_PM},
	{SEND_CMD_HEAD,CARD_COUNT_CMD,CARD_COUNT_RD},
	{SEND_CMD_HEAD,CARD_COUNT_CMD,CARD_COUNT_INIT},
	{SEND_CMD_HEAD,FIRMWARE_INFO_CMD,FIRMWARE_INFO_RD}
	
	
	
};


static unsigned char pack_buff[1024];

static unsigned char bcc_check(unsigned char *string,int len)
{
	char bccsum = 0;
	int i;
	for(i = 0; i < len; i++)
	{
		bccsum ^= string[i];
	}
	return bccsum;
	
}

int read_ack()
{
	unsigned char ack;
	serial_read(&ack,1);
	switch(ack)
	{
		case ACK:
			return 0;
			break;
		case NAK:
			return -1;
			break;
		case EOT:
			return 1;
			break;
		/*default:
			return -1;*/
			
	}
	return -1;
}

int send_ack(unsigned char ack)
{
	return serial_sendx(&ack, 1);
}

void read_data()
{
	
}
int read_pack()
{
	int ret;
	//int one_pack_flag = 0;
	int one_pack_num = 0;
	int rev_num = 4;
	
	unsigned char buff[1024];
	unsigned char *pbuf = pack_buff;
	
	ret = serial_read(buff, rev_num);
	printf("\nread data ret=%d\n",ret);
	
	if((ret >= 4)&&(buff[0] == START))
	{
		memcpy( pbuf, buff,ret);
		rev_num = buff[2]*256 + buff[3] + 2;
		one_pack_num = rev_num + 4;
	}
	else
	{
		printf("\nread pack failed!\n");
		return -1;
	}
		
	pbuf += ret;
	//rev_num -= ret;
	ret = serial_read(buff, rev_num);
	memcpy( pbuf, buff,ret);
	pbuf = NULL;
	return one_pack_num;
}

int return_check(unsigned char cmd,PCARD_INFO pack_info)
{
	//int pack_len;
	int ack = read_ack();
	if(ack == 0)
	{
		pack_info->pack_buff.pack_len = read_pack();
		memcpy(pack_info->pack_buff.pack_data_head, pack_buff,pack_info->pack_buff.pack_len); 
		pack_info->pack_buff.return_data_pos = pack_info->pack_buff.pack_data_head + 10;
		
		switch(cmd)
		{
			case RESET_CMD:
			
			break;
			case GET_STATUS_CMD:
			
			break;
			case MOVE_CARD_CMD:
			
			break;
			case SET_ENTER_CARD_CMD:
			
			break;
			case CARD_TYPE_CMD:
			
			break;
			case RF_CARD_OPERATE_CMD:
			
			break;
			default:
			break;
		}
		send_ack(ACK);
		
	}
	else if(ack == 1)
	{
		send_ack(EOT);
	}
	else
	{
		send_ack(NAK);
	}
	return ack;
}
int send_packn(unsigned char *head,unsigned char *cmd, int cmd_len,unsigned char *data,unsigned char *tail,int data_len)
{
	int ret;
	int pos = 0;
	int len;
	unsigned char buff[1024];
	
	//len = sizeof(head);
	memcpy(buff,head,7);
	
	pos += 7;
	//len = sizeof(cmd);
	if(cmd_len != 0)
	{
		memcpy(buff + pos,cmd,cmd_len);
		pos += cmd_len;
	}
	
	
	if(data_len != 0)
	{
		memcpy(buff + pos,data,data_len);
		pos += data_len;
	}
	
	
	//len = sizeof(tail);
	memcpy(buff + pos,tail,2);
	
	pos += 2;
	buff[pos-1] = bcc_check(buff,pos-1);
	printf("send_len=%d\n",pos);
	
	return serial_sendx(buff, pos);
}

int send_pack(unsigned char *pack,int pack_len)
{
	return serial_sendx(pack, pack_len);
	
}

void *card_pthread(void *arg)
{
	int ret;
	
	return NULL;
}
char *string = "enter command:\n \
				1:reset\n   \
				2:check\n \
				3:move card\n \
				4:move card out enable\n \
				5:card type\n \
				6:rf_card operate\n";
				
int test_card_thread()
{
	int cmt;
	int pm;
	int card_cmd;
	int n;
	int ack;
	int send_len;
	int pack_len;
	int data_len;
	int back_flag = 0;
	unsigned char head[7] = {0xF2,0x00,0x00,0x03,0x00,0x00,0x00};
	unsigned char cmd[3] = {0x00,0x00,0x00};
	unsigned char data[100] = {0x00};
	unsigned char tail[2] = {EXIT,0x00};
	unsigned char key = 0xf0;
	//unsigned char Tx_Data[50]={0xF2,0x00,0x00,0x03,SEND_CMD_HEAD,RESET_CMD,0x33,0x03,0x00};
	
	pthread_t pthread_id;
	seial_init(115200,8,'N',1);
	
	pthread_create( &pthread_id, NULL, &card_pthread, NULL );
	uart_test();
	
	//Tx_Data[8] = bcc_check(Tx_Data,8);
	//send_pack(Tx_Data, 9);
	
	while(1)
	{
		printf(string);
		scanf("%d",&cmt);
		
		switch(cmt)
		{
			case 1:         //reset
				back_flag = 1;
				head[4] = SEND_CMD_HEAD; 
				head[5] = RESET_CMD;
				while(back_flag == 1)
				{
					printf("\nreset!\nenter pm:(0,1,3;exit:10)");
					scanf("%d",&pm);
					
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					head[2] = 0;
					head[3] = 0x03;
					head[6] = '0' + pm;
					
					data_len = 0;
					send_packn(head,cmd,0, data,tail,data_len);
					
				}
				printf("exit\n");
				/*printf("\nreset!\nenter pm:(0,1,3)");
				scanf("%d",&pm);
				head[2] = 0;
				head[3] = 0x03;
				cmd[0] = 0x43; 
				cmd[1] = RESET_CMD;
				cmd[2] = '0' + pm;
				tail[0] = EXIT;
				data_len = 0;*/
				break;
			case 2:           //check
				head[4] = SEND_CMD_HEAD; 
				head[5] = GET_STATUS_CMD;
				back_flag  = 1;
				while(back_flag == 1)
				{
					printf("\ncheck card!\nenter pm:(0,1);exit:10");
					scanf("%d",&pm);
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					head[2] = 0;
					head[3] = 0x03;
					
					head[6] = '0' + pm;
					
					data_len = 0;
					send_packn(head,cmd,0, data,tail,data_len);
				}
				
				break;
			case 3:           //move card
				head[4] = SEND_CMD_HEAD; 
				head[5] = MOVE_CARD_CMD;
				back_flag = 1;
				while(back_flag == 1)
				{
					printf("\nmove card!\nenter pm:(0,1,2,3,9);exit:10");
					scanf("%d",&pm);
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					head[2] = 0;
					head[3] = 0x03;
					
					head[6] = '0' + pm;
					
					data_len = 0;
					send_packn(head,cmd,0, data,tail,data_len);
					
				}
				
				break;
			case 4:            //move card out enable
				head[4] = SEND_CMD_HEAD; 
				head[5] = SET_ENTER_CARD_CMD;
				back_flag  = 1;
				while(back_flag == 1)
				{
					printf("\nmove card out enable!\nenter pm:(0,1);exit:10");
					scanf("%d",&pm);
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					head[2] = 0;
					head[3] = 0x03;
					
					head[6] = '0' + pm;
					
					data_len = 0;
					send_packn(head,cmd,0, data,tail,data_len);
				}
				
				break;
			case 5:
				head[4] = SEND_CMD_HEAD; 
				head[5] = CARD_TYPE_CMD;
				back_flag  = 1;
				while(back_flag == 1)
				{
					printf("\ncard_type!\nenter pm:(0,1);exit:10");
					scanf("%d",&pm);
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					head[2] = 0;
					head[3] = 0x03;
					
					head[6] = '0' + pm;
					
					data_len = 0;
					send_packn(head,cmd,0, data,tail,data_len);
				}
				
				break;
			case 6:
				head[4] = SEND_CMD_HEAD; 
				head[5] = RF_CARD_OPERATE_CMD;
				back_flag = 1;
				while(back_flag == 1)
				{
					printf("\nRF card operate!\nenter pm: \
					\n0:reset\n1:power down\n2:check status\n3:card operate\n4:,5,9;exit:10");
					scanf("%d",&pm);
					if(pm == 0)
					{
						data[0] = 'A';
						data[1] = 'B';
					
						data_len = 2;
					}
					else if(pm == 1)
					{
						data_len = 0;
					}
					else if(pm == 2)
					{
						data_len = 0;
					}
					else if(pm == 3)
					{
						///back_flag = 2;
						//while(back_flag == 2)
						///{
							printf("enter operate cmd:\n1:check key\n2:read sector\n3:change key\n4:write sector\n5:key to eprom\n6:check key from eprom\n7:init value\n8:read value\n9:add value\n10:dec value\n");
							scanf("%d",&card_cmd);
							if(card_cmd == 1)
							{
								data[0] = 0x00;
								data[1] = 0x20;
								data[2] = 0x00;
								data[3] = 0x08;
								data[4] = 0x06;
								data[5] = 0xff;
								data[6] = 0xff;
								data[7] = 0xff;
								data[8] = 0xff;
								data[9] = 0xff;
								data[10] = 0xff;
								data_len = 11;
							}
							else if(card_cmd == 2)
							{
								data[0] = 0x00;
								data[1] = 0xB0;
								data[2] = 0x08;
								data[3] = 0x00;
								data[4] = 0x04;
								data_len = 5;
							}	
							else if(card_cmd == 3)
							{
								//printf("input key\n");
								//scanf("%d",&card_cmd);
								//key = key + card_cmd;
								data[0] = 0x00;
								data[1] = 0xD5;
								data[2] = 0x00;
								data[3] = 0x08;
								data[4] = 0x06;
								data[5] = 0xff;
								data[6] = 0xff;
								data[7] = 0xff;
								data[8] = 0xff;
								data[9] = 0xff;
								data[10] = 0xff;
								data_len = 11;
							}
							else if(card_cmd == 4)
							{
								data[0] = 0x00;
								data[1] = 0xD1;
								data[2] = 0x08;
								data[3] = 0x00;
								data[4] = 0x01;
								data[5] = 0xff;
								data[6] = 0xff;
								data[7] = 0xff;
								data[8] = 0xff;
								data[9] = 0xff;
								data[10] = 0xff;
								data[11] = 0xff;
								data[12] = 0x07;
								data[13] = 0x80;
								data[14] = 0x69;
								data[15] = 0xff;
								data[16] = 0xff;
								data[17] = 0xff;
								data[18] = 0xff;
								data[19] = 0xff;
								data[20] = 0xff;
								
								data_len = 21;
							}
							else if(card_cmd == 5)
							{
								data[0] = 0x00;
								data[1] = 0xD0;
								data[2] = 0x00;
								data[3] = 0x08;
								data[4] = 0x06;
								data[5] = 0xff;
								data[6] = 0xff;
								data[7] = 0xff;
								data[8] = 0xff;
								data[9] = 0xff;
								data[10] = 0xff;
								data_len = 11;
							}
							else if(card_cmd == 6)
							{
								data[0] = 0x00;
								data[1] = 0x21;
								data[2] = 0x00;
								data[3] = 0x08;
								
								data_len = 4;
							}
							else if(card_cmd == 7)
							{
								data[0] = 0x00;
								data[1] = 0xD2;
								data[2] = 0x08;
								data[3] = 0x00;
								data[4] = 0x04;
								data[5] = 0x11;
								data[6] = 0x22;
								data[7] = 0x33;
								data[8] = 0x44;
								
								data_len = 9;
							}
							else if(card_cmd == 8)
							{
								data[0] = 0x00;
								data[1] = 0xB1;
								data[2] = 0x08;
								data[3] = 0x00;
								
								
								data_len = 4;
							}
							else if(card_cmd == 9)
							{
								data[0] = 0x00;
								data[1] = 0xD3;
								data[2] = 0x08;
								data[3] = 0x00;
								data[4] = 0x04;
								data[5] = 0x01;
								data[6] = 0x02;
								data[7] = 0x03;
								data[8] = 0x04;
								
								data_len = 9;
							}
							else if(card_cmd == 10)
							{
								data[0] = 0x00;
								data[1] = 0xD4;
								data[2] = 0x08;
								data[3] = 0x00;
								data[4] = 0x04;
								data[5] = 0x01;
								data[6] = 0x02;
								data[7] = 0x03;
								data[8] = 0x04;
								
								data_len = 9;
							}
						//}
						
					}
					else if(pm == 4)   //
					{
						/*data[0] = 0x00;
						data[1] = 0xB0;
						data[2] = 0x07;
						data[3] = 0x00;
						data[4] = 0x01;
						data[5] = 0xff;
						data[6] = 0xff;
						data[7] = 0xff;
						data[8] = 0xff;
						data[9] = 0xff;
						data[10] = 0xff;
						data_len = 5;*/
					}
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					
					head[2] = 0;
					head[3] = data_len + 3;
					
					head[6] = '0' + pm;
					send_packn(head,cmd,0, data,tail,data_len);
					
				}
				
				break;
			case 7:
				back_flag = 1;
				head[4] = SEND_CMD_HEAD; 
				head[5] = CPU_CARD_OPERATE_CMD;
				while(back_flag == 1)
				{
					printf("\nreset!\nenter pm:(0,3,5;exit:10)");
					scanf("%d",&pm);
					
					if(pm == 10)
					{
						back_flag = 0;
						break;
					}
					
					head[2] = 0;
					head[3] = 0x03;
					head[6] = '0' + pm;
					
					data_len = 0;
					send_packn(head,cmd,0, data,tail,data_len);
					
				}
				break;
			default:
				break;
			
		}
		//send_packn(head,cmd, data,tail,data_len);
		/*send_len = Tx_Data[2] * 256 + Tx_Data[3] + 6;
		printf("cmd=%d send_len=%d\n",cmt,send_len);
		Tx_Data[send_len-1] = bcc_check(Tx_Data,send_len-1);
		send_pack(Tx_Data, send_len);*/
		/*usleep(10000);
		ack = read_ack();
		pack_len = read_pack();
		printf("ack = %d read_pack len = %d\n",ack,pack_len);
		for(n = 0; n < pack_len; n++)
		{
			printf("%x\t",pack_buff[n]);
		}
		//fflush(stdout);*/
	}
	
	return pthread_id;
	
}