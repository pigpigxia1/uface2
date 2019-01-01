
#include <string.h>
#include <stdio.h> 
#include <unistd.h>
#include "FM175XX.h"
#include "FM175XX_REG.h"
#include "uart.h"
#include "gpio.h"

#include "LPCD_API.h"
#include "LPCD_CFG.h"


unsigned char MCU_TO_PCD_TEST(void)
{
    volatile unsigned char ucRegVal;
   /* pcd_RST();                                                      // 硬件复位                 
    ucRegVal = spi_GetReg(ControlReg);
    spi_SetReg(ControlReg, 0x10);                                   // 启动读写器模式             
    ucRegVal = spi_GetReg(ControlReg);
    spi_SetReg(GsNReg, 0xF0 | 0x04);                                // CWGsN = 0xF; ModGsN = 0x4 
    ucRegVal = spi_GetReg(GsNReg);*/
	FM175XX_HardReset(); 
	GetReg(JREG_CONTROL,&ucRegVal);
	SetReg(JREG_CONTROL,0x10);
	GetReg(JREG_CONTROL,&ucRegVal);
	SetReg(0x27,0xF0 | 0x04);
	GetReg(0x27,&ucRegVal);
    if(ucRegVal != 0xF4)     		// 验证接口正确     
	{
		printf("check error\n");
		return ERROR;
	}
        
	printf("check sucess!\n");
    return SUCCESS;
}



int main(void)
{	  
unsigned char reg_data;

	//RCC_Config();			//时钟配置
	//GPIO_Config();		//GPIO配置
	
	//USART3_Config();	//UART3配置
	
	//SPI_Config();			//SPI配置
		
	//EXTI_Config();  	//外部中断配置
			
	//UART_Com_Para_Init(&UART_Com3_Para);    //初始化Uart1的通讯参数

	//Uart_Send_Msg("FM17522,FM17550 LPCD DEMO\r\n");
	//Uart_Send_Msg("Release Version 27_1 2017.06.16\r\n");
	
	//LED_ARM_WORKING_ON;
	rest_init();
	uart_init(9600,8,'N',1);
	
	FM175XX_HardReset();    //配置串行接口类型，我们的是通过硬件设置为Uart

	sleep(1);
	MCU_TO_PCD_TEST();
	
	GetReg(JREG_VERSION,&reg_data);
	//GetReg(0x01,&reg_data);
	if(reg_data == 0x88)
	{
		printf("IC Version = V03\n");
	}
		//Uart_Send_Msg("IC Version = V03\r\n");
	if(reg_data == 0x89)
	{
		printf("IC Version = V03+\n");
	}

	
		//Uart_Send_Msg("IC Version = V03+\r\n");
	//Uart_Send_Msg("Lpcd Debug Info Enable,Y or N?\r\n");
	
	/*if(Scan_User_Reply() == True)
		Lpcd_Debug_Info = 1;
	else
		Lpcd_Debug_Info = 0;
	
	Uart_Send_Msg("Lpcd Debug Test Enable,Y or N?\r\n");
	
	if(Scan_User_Reply() == True)
		Lpcd_Debug_Test = 1;
	else
		Lpcd_Debug_Test = 0;
	LED_ARM_WORKING_OFF;	*/
	//Lpcd_Debug_Info = 1;
	//Lpcd_Debug_Test = 1;
	//int cmd;
	/*while(1)
	{
		
		
		GetReg(JREG_VERSION,&reg_data);
		printf("reg_data=%x\n",reg_data);
		
		sleep(1);
		
		printf("poll\n");
	}
	gpio_unexport(228);*/

	if(Lpcd_Calibration_Event()== SUCCESS)//进行FM175XX校准		
	{
		Lpcd_Calibration_Backup();
		printf("Lpcd_Calibration_Event sucess!\n");
	}
	else
	{
		while(1)
		{
			printf("Lpcd_Calibration_Event failed!\n");
			sleep(1);
		}
		//首次校准失败，错误处理
	}
	Lpcd_Set_Mode(1);//进入LPCD模式	
	printf("enter lpcd\n");	
	while(1)
	{	
	//	PWR_EnterSTOPMode( PWR_Regulator_ON, PWR_STOPEntry_WFI); //MCU 进入STOP模式		
		
		
		Lpcd_Set_Mode(0);//退出LPCD 	
		Lpcd_Get_IRQ(&Lpcd.Irq);	
		Lpcd_IRQ_Event();
		Lpcd_Set_Mode(1);//进入LPCD模式		
		sleep(1);
	}

}








