/*
*********************************************************************************************************
*
*	ģ������ : APPģ��
*	�ļ����� : app.h
*	��    �� : V1.0
*	˵    �� :
*
*********************************************************************************************************
*/
#ifndef _ASP_H_
#define _ASP_H

#include "stm32f10x.h"

/* ����ϵͳ״̬ */
typedef enum
{
	SYS_POWER_ON = 0,

}WORK_MODE;

extern WORK_MODE Sys_Work_Mode;

void App_Init(void);
void App_Task(void);
#endif
