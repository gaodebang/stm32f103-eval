/*
*********************************************************************************************************
*
*	ģ������ : �����wifiģ������ģ��
*	�ļ����� : bsp_hlk_wifi.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_HLK_WIFI_H
#define _BSP_HLK_WIFI_H

#include "stm32f10x.h"

#define RCC_HLK_WIFI 	(RCC_APB2Periph_GPIOA)
#define PORT_HLK_WIFI  	(GPIOA)
#define PIN_HLK_WIFI	(GPIO_Pin_2)

#define HLK_WIFI_RST()	(PORT_HLK_WIFI->BRR = PIN_HLK_WIFI)
#define HLK_WIFI_SET()	(PORT_HLK_WIFI->BSRR = PIN_HLK_WIFI)

void bsp_InitHlkWifi(void);
void Hlk_Wifi_CheckOk(void);
void Hlk_Wifi_Quit_Pass_Through_Mode(void);

#endif
