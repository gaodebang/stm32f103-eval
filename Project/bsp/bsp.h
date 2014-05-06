/*
*********************************************************************************************************
*
*	ģ������ : BSPģ��
*	�ļ����� : bsp.h
*	��    �� : V1.0
*	˵    �� : ���ǵײ�����ģ�����е�h�ļ��Ļ����ļ��� Ӧ�ó���ֻ�� #include bsp.h ���ɣ�
*			   ����Ҫ#include ÿ��ģ��� h �ļ�
*
*********************************************************************************************************
*/
#ifndef _BSP_H_
#define _BSP_H


/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */


#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>

#ifndef TRUE
	#define TRUE  1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

/* ͨ��ȡ��ע�ͻ�������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_timer.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_wdg.h"
#include "bsp_uart_fifo.h"


/* �ṩ������C�ļ����õĺ��� */
void bsp_Init(void);
void bsp_Idle(void);
#endif