/*
*********************************************************************************************************
*
*	模块名称 : 看门狗模块
*	文件名称 : bsp_wdg.c
*	版    本 : V1.0
*	说    明 : 
*
*********************************************************************************************************
*/

#include "bsp.h"

#define WWDG_RELOAD_VALUE           0x7F    //窗口看门狗重装载值
#define WWDG_RELOAD_WTIME_VALUE     0x50    //窗口看门狗窗口上限值，此值应大于窗口看门狗下限值0x3F
#define WWDG_RELOAD_GAP             50      //单位ms

uint8_t WWDG_EN_MARK;												//标志窗口看门狗开启，窗口看门口初始化完成

/*
*********************************************************************************************************
*	函 数 名: bsp_InitWwdg
*	功能说明: 初始化窗口看门狗。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitWwdg(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* 打开窗口看门狗时钟，APB1低速时钟36MHz*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG , ENABLE);
    
    /* 使能看门狗中断 */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 设置窗口看门狗时钟为 WWDG_CLK = (PCLK1 (36MHz)/4096)/8 = 1152 Hz (~868 us), 最大超时时间: 780 * 64 = 58.25ms */
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    /* 设置窗口看门狗窗口上限, 计数器小于0x50时，才能重装载计数器 */  
    /* Enable WWDG and set counter value to 127。In this case the refresh window is: 
						WWDG_FRQ = 1/WWDG_CLK;
           ~WWDG_FRQ * (WWDG_RELOAD_VALUE-WWDG_RELOAD_WTIME_VALUE) < refresh window < ~WWDG_FRQ * (WWDG_RELOAD_VALUE-0x3F)
    */
    WWDG_SetWindowValue(WWDG_RELOAD_WTIME_VALUE);
    /* 使能窗口看门狗计数器，并设置计数初值为0x7F */
    WWDG_Enable(WWDG_RELOAD_VALUE);
    /* 清除窗口看门狗标志 */
    WWDG_ClearFlag();
    /* 使能窗口看门狗中断 */
    WWDG_EnableIT();
		/* 置位窗口看门口标志 */
		WWDG_EN_MARK = 1;
}
/*
*********************************************************************************************************
*	函 数 名: bsp_InitIwdg
*	功能说明: 初始化独立看门狗。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitIwdg(void)
{
    /* 打开独立看门狗时钟，LSI时钟32KHz*/
    RCC_LSICmd(ENABLE);
    /* 等待LSI时钟准备好 */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }
    /* 使能独立看门狗寄存器写操作 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* 设置独立看门狗时钟为 2KHz, 最大超时时间：2048ms */
    IWDG_SetPrescaler(IWDG_Prescaler_16);
    /* 设置独立看门狗重装载值为1S */
    IWDG_SetReload(2048-1);
    /* 重装载独立看门狗计数器 */
    IWDG_ReloadCounter();
    /* 使能独立看门狗计数器 */
    IWDG_Enable();
}
/*
*********************************************************************************************************
*	函 数 名: bsp_InitWdg
*	功能说明: 初始化看门狗。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitWdg(void)
{
    bsp_InitIwdg();
    bsp_InitWwdg();
}
/*
*********************************************************************************************************
*	函 数 名: Wwdg_ISR
*	功能说明: 窗口看门狗超时提前中断。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
void Wwdg_ISR(void)
{
	WWDG_ClearFlag();
	/*
	    处理窗口看门狗超时。
	*/
	Wwdg_Feed();
}
/*
*********************************************************************************************************
*	函 数 名: Wwdg_Feed
*	功能说明: 窗口看门狗超喂狗函数。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
void Wwdg_Feed(void)
{
    WWDG_SetCounter(WWDG_RELOAD_VALUE);
}

/*
*********************************************************************************************************
*	函 数 名: Iwdg_Feed
*	功能说明: 独立看门狗超喂狗函数。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
void Iwdg_Feed(void)
{
    IWDG_ReloadCounter();
}

/*
*********************************************************************************************************
*	函 数 名: Wwdg_Reload_Task
*	功能说明: 窗口看门狗超喂狗任务。
*	形    参:  
*	返 回 值: 无
*********************************************************************************************************
*/
void Wwdg_Reload_Task(void)
{
  static uint32_t sys_time, time_ms = 0, time_add_up = 0;
	const uint32_t work_time_max = WWDG_RELOAD_GAP;
	uint32_t offset_time_ticks = 0;
	uint8_t timeout_flag = 0;
	DISABLE_INT();  	/* 关中断 */
	sys_time = Sys_Time;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */
	ENABLE_INT();  		/* 开中断 */
	
	if(WWDG_EN_MARK)
	{
		if(time_ms != sys_time)
		{
			if(time_ms < sys_time)
			{
				offset_time_ticks = sys_time - time_ms;
				time_ms = sys_time;
				if(((uint64_t)offset_time_ticks  + (uint64_t)time_add_up) > UINT_LEAST32_MAX)
				{
					timeout_flag = 1;
					time_add_up = 0;
				}
				else if((offset_time_ticks  + time_add_up) > work_time_max)
				{
					timeout_flag = 1;
					time_add_up = 0;
				}
				else
				{
					time_add_up += offset_time_ticks;
				}
			}
			else
			{
				offset_time_ticks = UINT_LEAST32_MAX + sys_time - time_ms;
				time_ms = sys_time;
				if(((uint64_t)offset_time_ticks  + (uint64_t)time_add_up) > UINT_LEAST32_MAX)
				{
					timeout_flag = 1;
					time_add_up = 0;
				}
				else if((offset_time_ticks  + time_add_up) > work_time_max)
				{
					timeout_flag = 1;
					time_add_up = 0;
				}
				else
				{
					time_add_up += offset_time_ticks;
				}
			}		
			if(timeout_flag == 1)
			{
				timeout_flag = 0;
				Wwdg_Feed();
			}
		}
	}
}
