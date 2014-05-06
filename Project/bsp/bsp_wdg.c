/*
*********************************************************************************************************
*
*	ģ������ : ���Ź�ģ��
*	�ļ����� : bsp_wdg.c
*	��    �� : V1.0
*	˵    �� : 
*
*********************************************************************************************************
*/

#include "bsp.h"

#define WWDG_RELOAD_VALUE           0x7F    //���ڿ��Ź���װ��ֵ
#define WWDG_RELOAD_WTIME_VALUE     0x50    //���ڿ��Ź���������ֵ����ֵӦ���ڴ��ڿ��Ź�����ֵ0x3F
#define WWDG_RELOAD_GAP             50      //��λms

uint8_t WWDG_EN_MARK;												//��־���ڿ��Ź����������ڿ��ſڳ�ʼ�����

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitWwdg
*	����˵��: ��ʼ�����ڿ��Ź���
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitWwdg(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* �򿪴��ڿ��Ź�ʱ�ӣ�APB1����ʱ��36MHz*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG , ENABLE);
    
    /* ʹ�ܿ��Ź��ж� */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* ���ô��ڿ��Ź�ʱ��Ϊ WWDG_CLK = (PCLK1 (36MHz)/4096)/8 = 1152 Hz (~868 us), ���ʱʱ��: 780 * 64 = 58.25ms */
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    /* ���ô��ڿ��Ź���������, ������С��0x50ʱ��������װ�ؼ����� */  
    /* Enable WWDG and set counter value to 127��In this case the refresh window is: 
						WWDG_FRQ = 1/WWDG_CLK;
           ~WWDG_FRQ * (WWDG_RELOAD_VALUE-WWDG_RELOAD_WTIME_VALUE) < refresh window < ~WWDG_FRQ * (WWDG_RELOAD_VALUE-0x3F)
    */
    WWDG_SetWindowValue(WWDG_RELOAD_WTIME_VALUE);
    /* ʹ�ܴ��ڿ��Ź��������������ü�����ֵΪ0x7F */
    WWDG_Enable(WWDG_RELOAD_VALUE);
    /* ������ڿ��Ź���־ */
    WWDG_ClearFlag();
    /* ʹ�ܴ��ڿ��Ź��ж� */
    WWDG_EnableIT();
		/* ��λ���ڿ��ſڱ�־ */
		WWDG_EN_MARK = 1;
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitIwdg
*	����˵��: ��ʼ���������Ź���
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitIwdg(void)
{
    /* �򿪶������Ź�ʱ�ӣ�LSIʱ��32KHz*/
    RCC_LSICmd(ENABLE);
    /* �ȴ�LSIʱ��׼���� */
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }
    /* ʹ�ܶ������Ź��Ĵ���д���� */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* ���ö������Ź�ʱ��Ϊ 2KHz, ���ʱʱ�䣺2048ms */
    IWDG_SetPrescaler(IWDG_Prescaler_16);
    /* ���ö������Ź���װ��ֵΪ1S */
    IWDG_SetReload(2048-1);
    /* ��װ�ض������Ź������� */
    IWDG_ReloadCounter();
    /* ʹ�ܶ������Ź������� */
    IWDG_Enable();
}
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitWdg
*	����˵��: ��ʼ�����Ź���
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitWdg(void)
{
    bsp_InitIwdg();
    bsp_InitWwdg();
}
/*
*********************************************************************************************************
*	�� �� ��: Wwdg_ISR
*	����˵��: ���ڿ��Ź���ʱ��ǰ�жϡ�
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Wwdg_ISR(void)
{
	WWDG_ClearFlag();
	/*
	    �����ڿ��Ź���ʱ��
	*/
	Wwdg_Feed();
}
/*
*********************************************************************************************************
*	�� �� ��: Wwdg_Feed
*	����˵��: ���ڿ��Ź���ι��������
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Wwdg_Feed(void)
{
    WWDG_SetCounter(WWDG_RELOAD_VALUE);
}

/*
*********************************************************************************************************
*	�� �� ��: Iwdg_Feed
*	����˵��: �������Ź���ι��������
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Iwdg_Feed(void)
{
    IWDG_ReloadCounter();
}

/*
*********************************************************************************************************
*	�� �� ��: Wwdg_Reload_Task
*	����˵��: ���ڿ��Ź���ι������
*	��    ��:  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Wwdg_Reload_Task(void)
{
  static uint32_t sys_time, time_ms = 0, time_add_up = 0;
	const uint32_t work_time_max = WWDG_RELOAD_GAP;
	uint32_t offset_time_ticks = 0;
	uint8_t timeout_flag = 0;
	DISABLE_INT();  	/* ���ж� */
	sys_time = Sys_Time;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */
	ENABLE_INT();  		/* ���ж� */
	
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
