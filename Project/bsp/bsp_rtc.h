/*
*********************************************************************************************************
*
*	模块名称 : RTC模块
*	文件名称 : bsp_rtc.h
*	版    本 : V1.0
*	说    明 :
*
*********************************************************************************************************
*/
#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_

#include "bsp.h"

void RTC_DateShow(void);
void RTC_TimeShow(void);
void RTC_TimeStampShow(void);

void bsp_Rtc_Init(void);

#endif
