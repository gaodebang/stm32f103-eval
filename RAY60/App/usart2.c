#define		USART2_GLOBALS
#include "usart2.h"

/* ��ʱʱ�� */
#define TIME_OUT 10


/* ��ʼ���յ����ݱ�־ */
static COM_SATA Usart_State=com_idle;
/* ���ݼ��ʱ���жϱ�׼:t */
static u8 Rxdtime = 0;
/* �����ж��Ƿ���������Ҫ���� */
static u16 Txdconl = 0,Txdconh = 0;
/* �������ݻ����� */
static u8 Txddada[USART2_DATA_MAX_LEN];
/* �������ݻ����� */
static u8 Usart_Rxd_Tempdata[USART2_DATA_MAX_LEN];

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
	if(++datain >= USART2_DATA_MAX_LEN)
	{
		datain = 0;
	}
	return datain;
}
/*==================================================================================
* �� �� ���� USART2_Variable_Init
* ��    ���� None
* ��������:  ��ʼ������2��Ҫ��ʼ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART2_Variable_Init(void)
{
	USART2_RXDEND_CON = 0;
}
/*==================================================================================
* �� �� ���� USART2_Tx
* ��    ���� datain
* ��������:  �����ݷ��͵�����������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART2_Tx(u8 datain) 
{
	Txdconh = uart_dataup(Txdconh);
	Txddada[Txdconh] = datain;
}

/*==================================================================================
* �� �� ���� USART2_Tx_Chars
* ��    ���� *puchMsg, num
* ��������:  ��һ�����ݵ�num�����͵�����������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART2_Tx_Chars(u8 *puchMsg,u16 num)
{
	u16 i;
	USART2_Tx(SOF);
	for(i=0;i<num;i++)
	{
		if((*(puchMsg+i)==SOF)||(*(puchMsg+i)==EOF)||(*(puchMsg+i)==MARK))
		{
			USART2_Tx(MARK);
		}
		USART2_Tx(*(puchMsg+i));
	}
	USART2_Tx(EOF);
}

/*==================================================================================
* �� �� ���� USART2_IRQ_Deal
* ��    ���� None
* ��������:  Usart2�жϷ�����
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART2_IRQ_Deal(void)
{
		u8 temp_data;
    /////////////////////////////////////////////////////////////////////
    //
    //    UART 2 receive interrupt routine
    //
    /////////////////////////////////////////////////////////////////////
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
    	USART_ClearITPendingBit(USART2, USART_IT_RXNE);          // clear interrupt flag
			temp_data=USART_ReceiveData(USART2);
			if((Rxdtime == 0)&&(temp_data==SOF))
			{
				USART2_RXD_CON = 0;
				Rxdtime = TIME_OUT;
				Usart_Rxd_Tempdata[USART2_RXD_CON] = temp_data;
				USART2_RXD_CON++;
				Usart_State=com_normal;
			}
			else if(Usart_State==com_normal)
			{
				Rxdtime = TIME_OUT;
				if(temp_data==MARK)
				{
					Usart_State=com_mark;
				}
				else if(temp_data==EOF)
				{
					Usart_Rxd_Tempdata[USART2_RXD_CON] = temp_data;
					USART2_RXD_CON++;	
					Usart_State=com_eof;
				}
				else
				{
					Usart_Rxd_Tempdata[USART2_RXD_CON] = temp_data;
					USART2_RXD_CON++;
				}
			}
			else if(Usart_State==com_mark)
			{
				Rxdtime = TIME_OUT;
				Usart_Rxd_Tempdata[USART2_RXD_CON] = temp_data;
				USART2_RXD_CON++;
				Usart_State=com_normal;
			}			
    }

   /////////////////////////////////////////////////////////////////////
    //
    //    UART 2 transmit interrupt routine
    //
    /////////////////////////////////////////////////////////////////////
    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
    {
    	USART_ClearITPendingBit(USART2, USART_IT_TXE);          // reset interrupt flag
    }

    /////////////////////////////////////////////////////////////////////
    //
    //    UART 2 overrun interrupt
    //
    /////////////////////////////////////////////////////////////////////    
    if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
    {
        USART_ClearFlag(USART2,USART_FLAG_ORE);
        USART_ReceiveData(USART2);
    }
}
/*==================================================================================
* �� �� ���� USART2_Run
* ��    ���� None
* ��������:  Usart2ʵʱ��������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART2_Run(void)
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
			if(Usart_State==com_eof)
			{
				Usart_State=com_idle;
				if(USART2_RXD_CON>2)
				{
					USART2_RXD_CON-=2;
					for(i=0;i<USART2_RXD_CON;i++)
					{
						USART2_RXD_DATA[i] = Usart_Rxd_Tempdata[i+1]; //�����ݷŵ����ջ�����
					}
			   	USART2_RXDEND_CON++;
			  }
	    }
		}
	}
	if( Txdconl != Txdconh ) //�����в�ֵ�����������ݷ���
	{
		if(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == SET ) //���ݼĴ����շ���
		{
			Txdconl = uart_dataup(Txdconl);
			USART_SendData(USART2,Txddada[Txdconl]); 	/* ���ڵײ㷢�ͺ��� */
		}
	}
}