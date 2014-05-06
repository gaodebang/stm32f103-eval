/*
*********************************************************************************************************
*
*	ģ������ : �����wifiģ������ģ��
*	�ļ����� : bsp_hlk_wifi.c
*	��    �� : V1.0
*	˵    �� : �����wifiģ��Ĳ�����
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
*	�� �� ��: bsp_InitHlkWifi
*	����˵��: ʹwifiģ��ָ�����Ĭ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitHlkWifi(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_HLK_WIFI, ENABLE);
	/*
		�������е�����Ϊ©����·ģʽʱ��GPIO����Ĵ�����ֵȱʡ��0����˻Ḵλwifiģ��
		�����Ҳ�ϣ���ģ�����ڸı�GPIOΪ©����·ǰ������λ1
	*/
	HLK_WIFI_SET();

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO������ٶ� */
	GPIO_InitStructure.GPIO_Pin = PIN_HLK_WIFI;
	GPIO_Init(PORT_HLK_WIFI, &GPIO_InitStructure);
	Hlk_Wifi_CheckOk();
}


/*
*********************************************************************************************************
*	�� �� ��: Hlk_Wifi_CheckOk
*	����˵��: �ȴ�wifiģ���ʼ�����
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: Hlk_Wifi_Quit_Pass_Through_Mode
*	����˵��: ʹwifiģ���˳�͸��ģʽ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Hlk_Wifi_Quit_Pass_Through_Mode(void)
{
	HLK_WIFI_RST();
	bsp_DelayMS(200);//�����wifiģ��ʶ���˳��ź���Ҫ>=100ms
	HLK_WIFI_SET();
}

/*
*********************************************************************************************************
*	�� �� ��: Hlk_Wifi_Restore_Factory_Settings
*	����˵��: ʹwifiģ��ָ�����Ĭ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Hlk_Wifi_Restore_Factory_Settings(void)
{
	HLK_WIFI_RST();
	bsp_DelayMS(10000);//�����wifiģ��ʶ��ָ������ź���Ҫ>=6s
	HLK_WIFI_SET();
}

