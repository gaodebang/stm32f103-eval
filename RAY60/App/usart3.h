#ifndef _USART3_H_
#define _USART3_H_

#include "sys_config.h"

#ifdef		USART3_GLOBALS
#define   USART3_EXT
#else 
#define		USART3_EXT		extern 
#endif

#define USART3_DATA_MAX_LEN (1100)


/* ���յ�������֡���� */
USART3_EXT u16 	USART3_RXD_CON;
/* ���յ�������֡�����ۼӱ�ʶ */
USART3_EXT u8 	USART3_RXDEND_CON;
/* ���յ������ݴ���� */
USART3_EXT u8 	USART3_RXD_DATA[USART3_DATA_MAX_LEN];


void USART3_Variable_Init(void);
void USART3_Tx(u8 datain);
void USART3_Tx_Chars(u8 *puchMsg,u16 num);
void USART3_IRQ_Deal(void);
void USART3_Run(void);

#endif