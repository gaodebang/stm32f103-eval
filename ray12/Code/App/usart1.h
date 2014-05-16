#ifndef _USART1_H_
#define _USART1_H_

#include "sys_config.h"  

#ifdef		USART1_GLOBALS
#define   USART1_EXT
#else 
#define		USART1_EXT		extern 
#endif

#define USART1_DATA_MAX_LEN (520)


/* ���յ�������֡���� */
USART1_EXT u16 	USART1_RXD_CON;
/* ���յ�������֡�����ۼӱ�ʶ */
USART1_EXT u8 	USART1_RXDEND_CON;
/* ���յ������ݴ���� */
USART1_EXT u8 	USART1_RXD_DATA[USART1_DATA_MAX_LEN];


void USART1_Variable_Init(void);
void USART1_Tx(u8 datain);
void USART1_Tx_Chars(u8 *puchMsg,u16 num);
void USART1_IRQ_Deal(void);
void USART1_Run(void);

#endif