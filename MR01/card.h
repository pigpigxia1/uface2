#ifndef __CARD_H
#define __CARD_H

#define START  0XF2                   //开始位
#define EXIT   0X03                   //停止位
#define ACK    0X06                   //应答位
#define NAK    0X15                   //否认
#define EOT    0X04                   //取消清除字符

#define SEND_CMD_HEAD 0X43
#define RE_OK_CMD_HEAD 0X50          //返回数据正常
#define RE_ERR_CMD_HEAD 0X45         //返回数据错误

#define RESET_CMD    0X30
#define RESET_MOVE_TO_OUT     0X30
#define RESET_MOVE_TO_BOX     0X31
#define RESET_NO_MOVE         0X33
#define	RESET_MOVE_TO_OUT_C   0X34
#define RESET_MOVE_TO_BOX_C   0X35
#define RESET_NO_MOVE_C       0X37

#define GET_STATUS_CMD   0X31
#define GET_STATUS_BASE       0X30
#define	GET_STATUS_SENSOR     0X31

#define MOVE_CARD_CMD    0X32
#define MOVE_CARD_TO_OUT_HOLD     0X30     //持卡位
#define MOVE_CARD_TO_IC_POS   0X31
#define MOVE_CARD_TO_RF_POS   0X32
#define MOVE_CARD_TO_BOX      0X33
#define MOVE_CARD_TO_OUT_NHOLD     0X39     //不持卡位

#define SET_ENTER_CARD_CMD    0X33
#define ENTER_CARD_OK     0X30
#define ENTER_CARD_NO     0X31

#define CARD_TYPE_CMD    0X50
#define CHECK_IC_TYPE     0X30
#define CHECK_RF_TYPE     0X31

#define CPU_CARD_OPERATE_CMD  0X51
#define CPU_CARD_RESET    0X30
#define CPU_CARD_POWER_DOWN  0X31
#define CPU_CARD_CHECK_STATUS    0X32
#define CPU_CARD_APDU_0          0X33
#define CPU_CARD_APDU_1          0X34
#define CPU_CARD_HOT_RESET   0X38
#define CPU_CARD_AUTO_TELL_APDU  0X39

#define ASM_CARD_OPERATE_CMD  0X52
#define ASM_CARD_RESET    0X30
#define ASM_CARD_POWER_DOWN  0X31
#define ASM_CARD_CHECK_STATUS    0X32
#define ASM_CARD_APDU_0          0X33
#define ASM_CARD_APDU_1          0X34
#define ASM_CARD_HOT_RESET   0X38
#define ASM_CARD_AUTO_TELL_APDU  0X39
#define ASM_CARD_SELECT_POS      0X40

#define SEL_CARD_OPERATE_CMD  0X53
#define SEL_CARD_RESET    0X30
#define SEL_CARD_POWER_DOWN  0X31
#define SEL_CARD_CHECK_STATUS    0X32
#define SEL_CARD_4442          0X33
#define SEL_CARD_4428          0X34

#define I2C_CARD_OPERATE_CMD  0X54
#define I2C_CARD_RESET    0X30
#define I2C_CARD_POWER_DOWN  0X31
#define I2C_CARD_CHECK_STATUS    0X32
#define I2C_CARD_READ          0X33
#define I2C_CARD_WRITE          0X34

#define RF_CARD_OPERATE_CMD  0X60
#define RF_CARD_ACTIV    0X30
#define RF_CARD_POWER_DOWN  0X31
#define RF_CARD_CHECK_STATUS    0X32
#define RF_CARD_MAFARE_RDWR          0X33
#define RF_CARD_A_CL_DATA_SWAP        0X34
#define RF_CARD_B_CL_DATA_SWAP        0X35
#define RF_CAED_WAKE_SLEEP            0X39

#define CARD_MACHINE_ID_RD_CMD     0XA2
#define CAED_MACHINE_ID_RD_PM      0X30

#define MACHINE_CONF_INFO_CMD      0XA3
#define MACHINE_CONF_INFO_PM       0X30

#define CARD_COUNT_CMD    0XA5
#define CARD_COUNT_RD     0X30
#define CARD_COUNT_INIT   0X31

#define FIRMWARE_INFO_CMD  0XDC 
#define FIRMWARE_INFO_RD   0X31

typedef enum{
	NO_RF_CARD_ACTIVE   = 0x0,
	RF_MIFARE_S50		= 0x01,
	RF_MIFARE_S70		= 0x02,
	RF_MIFARE_UL		= 0x03,
	RF_TYPE_A_CPU		= 0x04,
	RF_TYPE_B_CPU		= 0x05
} CARD_TYPR;

typedef enum{
	ERR = 0,
	OK	= 1
	
} BOOL;

typedef enum{
	AA
} CARD_MACHINE_STARUS;

typedef struct{
		unsigned char *pack_data_head;
		unsigned char *return_data_pos;
		unsigned char *send_data_pos;
		int pack_len;
		int data_len;
} PACK_BUFF, *PPACK_BUFF;      //数据包信息

typedef struct{
	BOOL power_on;
	CARD_TYPR card_type;
	unsigned char uid[10];
	char uid_len;
	char rtype;
	char key_type;
	char sector_num;
	char passwd[6];
	PACK_BUFF pack_buff;

} CARD_INFO,*PCARD_INFO;    //卡信息

typedef struct{
	BOOL (*reset)(PCARD_INFO card_info);
	BOOL (*power_down)(PCARD_INFO card_info);
	BOOL (*check_type)(PCARD_INFO card_info);
	BOOL (*check_passwd)(PCARD_INFO card_info);
	BOOL (*check_passwd_from_eprom)(PCARD_INFO card_info);
	BOOL (*modify_passwd)(PCARD_INFO card_info);
	BOOL (*load_passwd_to_eprom)(PCARD_INFO card_info);
	BOOL (*read_sector_block)(PCARD_INFO card_info);
	BOOL (*write_sector_block)(PCARD_INFO card_info);
	BOOL (*sector_init)(PCARD_INFO card_info);
	BOOL (*read_sector_line)(PCARD_INFO card_info);
	BOOL (*add_data_to_sector)(PCARD_INFO card_info);
	BOOL (*dec_data_to_sector)(PCARD_INFO card_info);
	//BOOL (*load_passwd_to_eprom)(PCARD_INFO card_info);
}CARD_OPS;



#endif