#ifndef _USART2_H_
#define _USART2_H_

#include "sys_config.h"  

#ifdef		USART2_GLOBALS
#define   USART2_EXT
#else 
#define		USART2_EXT		extern 
#endif

#define USART2_DATA_MAX_LEN (1100)


/* ���յ�������֡���� */
USART2_EXT u16 	USART2_RXD_CON;
/* ���յ�������֡�����ۼӱ�ʶ */
USART2_EXT u8 	USART2_RXDEND_CON;
/* ���յ������ݴ���� */
USART2_EXT u8 	USART2_RXD_DATA[USART2_DATA_MAX_LEN];


void USART2_Variable_Init(void);
void USART2_Tx(u8 datain);
void USART2_Tx_Chars(u8 *puchMsg,u16 num);
void USART2_IRQ_Deal(void);
void USART2_Run(void);

#endif