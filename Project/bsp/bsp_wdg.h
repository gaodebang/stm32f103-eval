/*
*********************************************************************************************************
*
*	ģ������ : ���Ź�ģ��
*	�ļ����� : bsp_wdg.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef __BSP_WDG_H
#define __BSP_WDG_H

#include "stm32f10x.h"

extern uint8_t WWDG_EN_MARK;

void bsp_InitWdg(void);
void Wwdg_ISR(void);
void Wwdg_Reload_Task(void);

void Wwdg_Feed(void);
void Iwdg_Feed(void);
#endif


