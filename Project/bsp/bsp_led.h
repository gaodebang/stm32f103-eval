/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*	�ļ����� : bsp_led.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

void bsp_InitLed(void);
void bsp_LedOn(uint8_t No);
void bsp_LedOff(uint8_t No);
void bsp_LedToggle(uint8_t No);
uint8_t bsp_IsLedOn(uint8_t No);

void bsp_LedTask(void);

#endif
