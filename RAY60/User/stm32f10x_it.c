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
* �� �� ���� WWDG_IRQHandler
* ��    ���� None
* ��������:  ���ڿ��Ź�Early wakeup interrupt
* �� �� ֵ�� None
* ��    ע�� 1���ж�˵��ϵͳ���в��������ϣ�1�����Ź��������ڣ����ڿ��Ź���Ҫ��λ�ˣ���
						 ����ӱ�ʶ���ж�ϵͳ����״̬�����Ƽ��ڴ˴�ι����
						 2���ڴ�ι��һ���̶���ʧȥ�˿��Ź������壬��������ϵͳ�������С�
						 3������жϱ�־�븴λ������ֵ���Ⱥ�˳��Դ˺�����Ӱ�죨����֤...����
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
void WWDG_IRQHandler(void)
{
	WWDG_ClearFlag();
	Reset_Wwdg();
}

/*==================================================================================
* �� �� ���� SysTick_Handler
* ��    ���� None
* ��������:  ϵͳ��શ�ʱ��updata interrupt
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
void SysTick_Handler(void)
{
	Timingdelay_Decrement();							//Delay_Ms()��ʱ����ˢ�£���һ��
	SYS_TIME_MS++;												//ϵͳʱ������
}
/*==================================================================================
* �� �� ���� TIM2_IRQHandler
* ��    ���� None
* ��������:  TIM2 updata interrupt
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
void TIM2_IRQHandler(void)
{
	TIM_ClearFlag(TIM2, TIM_IT_Update);		//���TIM2 updata�жϱ�־λ
}
/*==================================================================================
* �� �� ���� TIM4_IRQHandler
* ��    ���� None
* ��������:  TIM4 updata interrupt
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void TIM4_IRQHandler(void)
{
	TIM_ClearFlag(TIM4, TIM_IT_Update);		//���TIM4 updata�жϱ�־λ
}
/*==================================================================================
* �� �� ���� USART1_IRQHandler
* ��    ���� None
* ��������:  USART1 interrupt
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART1_IRQHandler(void)
{
	USART1_IRQ_Deal();										//����1�жϷ�������
}
/*==================================================================================
* �� �� ���� USART2_IRQHandler
* ��    ���� None
* ��������:  USART2 interrupt
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART2_IRQHandler(void)
{
	USART2_IRQ_Deal();										//����2�жϷ�������
}
/*==================================================================================
* �� �� ���� USART3_IRQHandler
* ��    ���� None
* ��������:  USART3 interrupt
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void USART3_IRQHandler(void)
{
	USART3_IRQ_Deal();										//����3�жϷ�������
}
