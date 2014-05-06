/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ��ģ��
*	�ļ����� : bsp_timer.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "stm32f10x.h"

extern volatile uint32_t Sys_Time;

void bsp_DelayMS(uint32_t n);
void bsp_DelayUS(uint32_t n);
uint32_t bsp_GetRunTime(void);

void bsp_InitTimer(void);
void SysTick_ISR(void);

void Ceshi_Task0(void);
void Ceshi_Task1(void);
#endif
