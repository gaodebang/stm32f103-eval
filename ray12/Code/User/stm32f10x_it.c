/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval : None
  * Cortex.-M3 NMI (Non-Maskable Interrupt) 
  */
void NMI_Handler(void)
{
    if(RCC_GetITStatus(RCC_IT_CSS) == SET)
    {
        RCC_ClearITPendingBit(RCC_IT_CSS);
    }
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/*==================================================================================
* 函 数 名： SysTick_Handler
* 参    数： None
* 功能描述:  系统嘀嗒定时器updata interrupt
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/ 
void SysTick_Handler(void)
{
	Timingdelay_Decrement();							//Delay_Ms()延时计数刷新（减一）
	SYS_TIME_MS++;												//系统时基更新
}

/*==================================================================================
* 函 数 名： TIM2_IRQHandler
* 参    数： None
* 功能描述:  TIM2 updata interrupt
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/ 
void TIM2_IRQHandler(void)
{
	TIM_ClearFlag(TIM2, TIM_IT_Update);		//清除TIM2 updata中断标志位
	if(++guanji_times_add_up>=5)
	{
		guanji_times_add_up=0;
		EnterStandby();
	}
}
/*==================================================================================
* 函 数 名： TIM4_IRQHandler
* 参    数： None
* 功能描述:  TIM4 updata interrupt
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void TIM4_IRQHandler(void)
{
	TIM_ClearFlag(TIM4, TIM_IT_Update);		//清除TIM4 updata中断标志位
}
/*==================================================================================
* 函 数 名： USART1_IRQHandler
* 参    数： None
* 功能描述:  USART1 interrupt
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void USART1_IRQHandler(void)
{
	USART1_IRQ_Deal();										//串口1中断服务处理函数
}
/*==================================================================================
* 函 数 名： USART2_IRQHandler
* 参    数： None
* 功能描述:  USART2 interrupt
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void USART2_IRQHandler(void)
{
	USART2_IRQ_Deal();										//串口2中断服务处理函数
}
