#include <stdio.h>
#include "card.h"
#include "card_machine.h"

static unsigned char head[7] = {0xF2,0x00,0x00,0x03,0x00,0x00,0x00};
static unsigned char tail[2] = {0x03,0x00};


static void set_card_type(PCARD_INFO card_info)
{
	if(*(card_info->pack_buff.return_data_pos) == 0x4D)
	{
		if(*(card_info->pack_buff.return_data_pos + 1) == 0x04)
		{
			card_info->card_type = RF_MIFARE_S50;
		}	
		else if(*(card_info->pack_buff.return_data_pos + 1) == 0x02)
		{
			card_info->card_type = RF_MIFARE_S70;
		}
		card_info->uid_len = *(card_info->pack_buff.return_data_pos + 2);
		memcpy(card_info->uid, card_info->pack_buff.return_data_pos + 3,card_info->uid_len); 
		
	}
}
static BOOL card_reset(PCARD_INFO card_info)
{
	
	send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	if(return_check(0,card_info) == 0)
	{
		set_card_type(card_info);
		return OK;
	}
	
	return ERR;
	
}

static BOOL card_power_down(PCARD_INFO card_info)
{
	send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	if(return_check(0,card_info) == 0)
	{
		return OK;
	}
	
	return ERR;
	
}

static CARD_TYPR check_card_type(PCARD_INFO card_info)
{
	CARD_TYPR card_type;
	send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == '0')
		{
			if(*(card_info->pack_buff.return_data_pos + 1) == '0')
				card_type = NO_RF_CARD_ACTIVE;
		}
		else if(*(card_info->pack_buff.return_data_pos) == '1')
		{
			if(*(card_info->pack_buff.return_data_pos + 1) == '0')
			{
				card_type = RF_MIFARE_S50;
			}
			else if(*(card_info->pack_buff.return_data_pos + 1) == '1')
			{
				card_type = RF_MIFARE_S70;
			}
			else if(*(card_info->pack_buff.return_data_pos + 1) == '2')
			{
				card_type = RF_MIFARE_UL;
			}
		}
	}
	return card_type;
}

static BOOL check_passwd(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0x20} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	
	return ERR;
}

static BOOL check_passwd_from_eprom(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0x21} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL modify_passwd(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xD5,0x00} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 3;
	send_packn(card_info->pack_buff.pack_data_head,cmd,3, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL load_passwd_to_eprom(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xD0} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		//if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL read_sector_block(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xB0} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		//if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL write_sector_block(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xD1} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		//if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL sector_init(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xD2} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL read_sector_line(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xB1} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		//if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

static BOOL add_data_to_sector(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xD3} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}
static BOOL dec_data_to_sector(PCARD_INFO card_info)
{
	unsigned char cmd[2] = {0x00,0xD4} ;
	//send_pack(card_info->pack_buff.pack_data_head,card_info->pack_buff.pack_len);
	card_info->pack_buff.pack_len += 2;
	send_packn(card_info->pack_buff.pack_data_head,cmd,2, card_info->pack_buff.send_data_pos,tail,card_info->pack_buff.data_len);
	if(return_check(0,card_info) == 0)
	{
		if(*(card_info->pack_buff.return_data_pos) == 0x90 && *(card_info->pack_buff.return_data_pos + 1) == 0x0)
			return OK;
	}
	return ERR;
}

int card_init(PCARD_INFO card_info)
{
	
		return 0;
}
