/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
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
