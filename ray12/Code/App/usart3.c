#define		USART3_GLOBALS
#include "usart3.h"

/* 开始接收到数据标志 */
static FlagStatus Usart_Rxd_Start=RESET;
/* 数据间隔时间判断标准:t */
static u8 Rxdtime = 0;
/* 用于判断是否有数据需要发送 */
static u16 Txdconl = 0,Txdconh = 0;
/* 发送数据缓冲区 */
static u8 Txddada[USART3_DATA_MAX_LEN];
/* 接收数据缓冲区 */
static u8 Usart_Rxd_Tempdata[USART3_DATA_MAX_LEN];

/*==================================================================================
* 函 数 名： uart_dataup
* 参    数： datain
* 功能描述:  固定加1操作
* 返 回 值： “datain+1”
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
* 函 数 名： USART3_Variable_Init
* 参    数： None
* 功能描述:  初始化串口3需要初始化参数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void USART3_Variable_Init(void)
{
	USART3_RXDEND_CON = 0;
}
/*==================================================================================
* 函 数 名： USART3_Tx
* 参    数： datain
* 功能描述:  将数据发送到缓冲区待发
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void USART3_Tx(u8 datain) 
{
	Txdconh = uart_dataup(Txdconh);
	Txddada[Txdconh] = datain;
}

/*==================================================================================
* 函 数 名： USART3_Tx_Chars
* 参    数： *puchMsg, num
* 功能描述:  将一组数据的num个发送到缓冲区待发
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
* 函 数 名： USART3_IRQ_Deal
* 参    数： None
* 功能描述:  Usart3中断服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
* 函 数 名： USART3_Run
* 参    数： None
* 功能描述:  Usart3实时运行任务
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
					USART3_RXD_DATA[i] = Usart_Rxd_Tempdata[i]; //将数据放到接收缓冲区
				}		
		  	USART3_RXDEND_CON++;
		  }
		}
	}
	if( Txdconl != Txdconh ) //发行有差值即可启动数据发送
	{
		if(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == SET ) //数据寄存器空发送
		{
			Txdconl = uart_dataup(Txdconl);
			USART_SendData(USART3,Txddada[Txdconl]); 	/* 串口底层发送函数 */
		}
	}
}