#define		USART2_GLOBALS
#include "usart2.h"

/* 超时时间 */
#define TIME_OUT 10


/* 开始接收到数据标志 */
static COM_SATA Usart_State=com_idle;
/* 数据间隔时间判断标准:t */
static u8 Rxdtime = 0;
/* 用于判断是否有数据需要发送 */
static u16 Txdconl = 0,Txdconh = 0;
/* 发送数据缓冲区 */
static u8 Txddada[USART2_DATA_MAX_LEN];
/* 接收数据缓冲区 */
static u8 Usart_Rxd_Tempdata[USART2_DATA_MAX_LEN];

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
	if(++datain >= USART2_DATA_MAX_LEN)
	{
		datain = 0;
	}
	return datain;
}
/*==================================================================================
* 函 数 名： USART2_Variable_Init
* 参    数： None
* 功能描述:  初始化串口2需要初始化参数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void USART2_Variable_Init(void)
{
	USART2_RXDEND_CON = 0;
}
/*==================================================================================
* 函 数 名： USART2_Tx
* 参    数： datain
* 功能描述:  将数据发送到缓冲区待发
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void USART2_Tx(u8 datain) 
{
	Txdconh = uart_dataup(Txdconh);
	Txddada[Txdconh] = datain;
}

/*==================================================================================
* 函 数 名： USART2_Tx_Chars
* 参    数： *puchMsg, num
* 功能描述:  将一组数据的num个发送到缓冲区待发
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
* 函 数 名： USART2_IRQ_Deal
* 参    数： None
* 功能描述:  Usart2中断服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
* 函 数 名： USART2_Run
* 参    数： None
* 功能描述:  Usart2实时运行任务
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
						USART2_RXD_DATA[i] = Usart_Rxd_Tempdata[i+1]; //将数据放到接收缓冲区
					}
			   	USART2_RXDEND_CON++;
			  }
	    }
		}
	}
	if( Txdconl != Txdconh ) //发行有差值即可启动数据发送
	{
		if(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == SET ) //数据寄存器空发送
		{
			Txdconl = uart_dataup(Txdconl);
			USART_SendData(USART2,Txddada[Txdconl]); 	/* 串口底层发送函数 */
		}
	}
}