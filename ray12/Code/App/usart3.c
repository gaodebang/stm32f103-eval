#define		USART3_GLOBALS
#include "usart3.h"

/* ��ʼ���յ����ݱ�־ */
static FlagStatus Usart_Rxd_Start=RESET;
/* ���ݼ��ʱ���жϱ�׼:t */
static u8 Rxdtime = 0;
/* �����ж��Ƿ���������Ҫ���� */
static u16 Txdconl = 0,Txdconh = 0;
/* �������ݻ����� */
static u8 Txddada[USART3_DATA_MAX_LEN];
/* �������ݻ����� */
static u8 Usart_Rxd_Tempdata[USART3_DATA_MAX_LEN];

/*==================================================================================
* �� �� ���� uart_dataup
* ��    ���� datain
* ��������:  �̶���1����
* �� �� ֵ�� ��datain+1��
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u16 uart_dataup(u16 datain)
{
	if(++datain >= USART3_DATA_MAX_LEN)
	{
		datain = 0;
	}
	return datain;
}
/*==================================================================================
* �� �� ���� USART3_Variable_Init
* ��    ���� None
* ��������:  ��ʼ������3��Ҫ��ʼ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART3_Variable_Init(void)
{
	USART3_RXDEND_CON = 0;
}
/*==================================================================================
* �� �� ���� USART3_Tx
* ��    ���� datain
* ��������:  �����ݷ��͵�����������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART3_Tx(u8 datain) 
{
	Txdconh = uart_dataup(Txdconh);
	Txddada[Txdconh] = datain;
}

/*==================================================================================
* �� �� ���� USART3_Tx_Chars
* ��    ���� *puchMsg, num
* ��������:  ��һ�����ݵ�num�����͵�����������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART3_Tx_Chars(u8 *puchMsg,u16 num)
{
	u16 i;
	for(i=0;i<num;i++)
	{
		USART3_Tx(*(puchMsg+i));
	}
}
/*==================================================================================
* �� �� ���� USART3_IRQ_Deal
* ��    ���� None
* ��������:  Usart3�жϷ�����
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART3_IRQ_Deal(void)
{
		u8 temp_data;
    /////////////////////////////////////////////////////////////////////
    //
    //    UART 3 receive interrupt routine
    //
    /////////////////////////////////////////////////////////////////////
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
    	USART_ClearITPendingBit(USART3, USART_IT_RXNE);          // clear interrupt flag
    	temp_data=USART_ReceiveData(USART3);
			if(Rxdtime == 0)
			{
				USART3_RXD_CON = 0;
				Usart_Rxd_Start=SET;
			}
			if(Usart_Rxd_Start==SET)
			{
				Rxdtime = 3;
				Usart_Rxd_Tempdata[USART3_RXD_CON] = temp_data;
				USART3_RXD_CON++;
			}
    }

    /////////////////////////////////////////////////////////////////////
    //
    //    UART 3 transmit interrupt routine
    //
    /////////////////////////////////////////////////////////////////////
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
    	USART_ClearITPendingBit(USART3, USART_IT_TXE);          // reset interrupt flag
    }

    /////////////////////////////////////////////////////////////////////
    //
    //    UART 3 overrun interrupt
    //
    /////////////////////////////////////////////////////////////////////    
    if(USART_GetFlagStatus(USART3, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USART3,USART_FLAG_ORE);
        USART_ReceiveData(USART3);
    }
}
/*==================================================================================
* �� �� ���� USART3_Run
* ��    ���� None
* ��������:  Usart3ʵʱ��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART3_Run(void)
{
	u16 i;
	u8 sys_time_ms;
	static u8 time_ms = 0;
	
	__disable_irq();
	sys_time_ms = SYS_TIME_MS;
	__enable_irq();	
	if(time_ms != sys_time_ms)
	{
		time_ms = sys_time_ms;
		if(Rxdtime > 0)
		{
			Rxdtime--;
		}
		else
		{
			if(Usart_Rxd_Start==SET)
			{
				Usart_Rxd_Start=RESET;
				for(i=0;i<USART3_RXD_CON;i++)
				{
					USART3_RXD_DATA[i] = Usart_Rxd_Tempdata[i]; //�����ݷŵ����ջ�����
				}		
		  	USART3_RXDEND_CON++;
		  }
		}
	}
	if( Txdconl != Txdconh ) //�����в�ֵ�����������ݷ���
	{
		if(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == SET ) //���ݼĴ����շ���
		{
			Txdconl = uart_dataup(Txdconl);
			USART_SendData(USART3,Txddada[Txdconl]); 	/* ���ڵײ㷢�ͺ��� */
		}
	}
}