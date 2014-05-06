/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.c
*	版    本 : V1.0
*	说    明 : 驱动LED指示灯
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
		LD1     : PA8          (低电平点亮，高电平熄灭)
		LD2     : PD2          (低电平点亮，高电平熄灭)
*/

/* 按键口对应的RCC时钟 */

#define RCC_ALL_LED 	(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD)

#define GPIO_PORT_LED1  GPIOA
#define GPIO_PIN_LED1	GPIO_Pin_8

#define GPIO_PORT_LED2  GPIOD
#define GPIO_PIN_LED2	GPIO_Pin_2
/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_LED, ENABLE);
	/*
		配置所有的LED指示灯GPIO为推挽输出模式
		由于将GPIO设置为输出时，GPIO输出寄存器的值缺省是0，因此会驱动LED点亮.
		这是我不希望的，因此在改变GPIO为输出前，先关闭LED指示灯
	*/
	bsp_LedOff(1);
	bsp_LedOff(2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
	GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		/* 设为输出口 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	/* IO口最大速度 */

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2;
	GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  No : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t No)
{
	No--;
	if (No == 0)
	{
		GPIO_PORT_LED1->BRR = GPIO_PIN_LED1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参:  No : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t No)
{
	No--;
	if (No == 0)
	{
		GPIO_PORT_LED1->BSRR = GPIO_PIN_LED1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参:  No : 指示灯序号，范围 1 - 4
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t No)
{
	if (No == 1)
	{
		GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_IsLedOn
*	功能说明: 判断LED指示灯是否已经点亮。
*	形    参:  No : 指示灯序号，范围 1 - 4
*	返 回 值: 1表示已经点亮，0表示未点亮
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t No)
{
	if (No == 1)
	{
		if ((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) != 0)
		{
			return 1;
		}
		return 0;
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedTask
*	功能说明: 
*	形    参: 
*	返 回 值: 
*********************************************************************************************************
*/
void bsp_LedTask(void)
{
	static uint32_t sys_time, time_ms = 0, time_add_up = 0;
	const uint32_t work_time_max = 500;
	uint32_t offset_time_ticks = 0;
	uint8_t timeout_flag = 0;
	
	DISABLE_INT();  	/* 关中断 */
	sys_time = Sys_Time;	/* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */
	ENABLE_INT();  		/* 开中断 */
	
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
			bsp_LedToggle(1);
		}
  }
}

