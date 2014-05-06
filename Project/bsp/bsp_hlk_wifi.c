/*
*********************************************************************************************************
*
*	模块名称 : 海陵科wifi模块驱动模块
*	文件名称 : bsp_hlk_wifi.c
*	版    本 : V1.0
*	说    明 : 海凌科wifi模块的操作。
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "app.h"


const uint8_t Hlk_Wifi_At[] = "at";
const uint8_t Hlk_Wifi_At_Netmode[] = "at+netmode=";
const uint8_t Hlk_Wifi_At_Reconn[] = "at+reconn=1\r\n";

/*
*********************************************************************************************************
*	函 数 名: bsp_InitHlkWifi
*	功能说明: 使wifi模块恢复出厂默认设置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitHlkWifi(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_HLK_WIFI, ENABLE);
	/*
		配置所有的引脚为漏极开路模式时，GPIO输出寄存器的值缺省是0，因此会复位wifi模块
		这是我不希望的，因此在改变GPIO为漏极开路前设置其位1
	*/
	HLK_WIFI_SET();

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */
	GPIO_InitStructure.GPIO_Pin = PIN_HLK_WIFI;
	GPIO_Init(PORT_HLK_WIFI, &GPIO_InitStructure);
	Hlk_Wifi_CheckOk();
}


/*
*********************************************************************************************************
*	函 数 名: Hlk_Wifi_CheckOk
*	功能说明: 等待wifi模块初始化完成
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Hlk_Wifi_CheckOk(void)
{
	uint8_t temp_rcv_data;
	while(1)
	{
		Hlk_Wifi_Quit_Pass_Through_Mode();
		comSendChar(COM1, 'a');
		if(comGetChar(COM1, &temp_rcv_data))
		{
			Sys_Work_Mode = HLK_WIFI_OK;
			break;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: Hlk_Wifi_Quit_Pass_Through_Mode
*	功能说明: 使wifi模块退出透传模式
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Hlk_Wifi_Quit_Pass_Through_Mode(void)
{
	HLK_WIFI_RST();
	bsp_DelayMS(200);//海凌科wifi模块识别退出信号需要>=100ms
	HLK_WIFI_SET();
}

/*
*********************************************************************************************************
*	函 数 名: Hlk_Wifi_Restore_Factory_Settings
*	功能说明: 使wifi模块恢复出厂默认设置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Hlk_Wifi_Restore_Factory_Settings(void)
{
	HLK_WIFI_RST();
	bsp_DelayMS(10000);//海凌科wifi模块识别恢复出厂信号需要>=6s
	HLK_WIFI_SET();
}

