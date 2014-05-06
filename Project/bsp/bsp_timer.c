/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ��ģ��
*	�ļ����� : bsp_timer.c
*	��    �� : V1.0
*	˵    �� : ����systick��ʱ����Ϊϵͳ�δ�ʱ����ȱʡ��ʱ����Ϊ1ms��
*			   ʵ����ms�����ӳٺ���������1ms�� ��us���ӳٺ���
*			   ʵ����ϵͳ����ʱ�亯����1ms��λ��
*
*********************************************************************************************************
*/

#include "bsp.h"

volatile uint32_t Sys_Time = 0;

static volatile uint32_t DelayCount = 0;
static volatile uint8_t TimeOutFlag = 0;

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayMS
*	����˵��: ms���ӳ٣��ӳپ���Ϊ����1ms
*	��    ��:  n : �ӳٳ��ȣ���λ1 ms�� n Ӧ����2
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t n)
{
	if (n == 0)
	{
		return;
	}
	else if (n == 1)
	{
		n = 2;
	}

	DISABLE_INT();  			/* ���ж� */

	DelayCount = n;
	TimeOutFlag = 0;

	ENABLE_INT();  				/* ���ж� */

	while (1)
	{
		bsp_Idle();				/* CPU����ִ�еĲ����� �� bsp.c �� bsp.h �ļ� */
		if (TimeOutFlag == 1)
		{
			break;
		}
	}
}

/*
*********************************************************************************************************
*    �� �� ��: bsp_DelayUS
*    ����˵��: us���ӳ١� ������systick��ʱ����������ܵ��ô˺�����
*    ��    ��:  n : �ӳٳ��ȣ���λ1 us
*    �� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
		reload = SysTick->LOAD;                
    ticks = n * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* �ս���ʱ�ļ�����ֵ */

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {    
            /* SYSTICK��һ���ݼ��ļ����� */    
            if (tnow < told)
            {
                tcnt += told - tnow;    
            }
            /* ����װ�صݼ� */
            else
            {
                tcnt += reload - tnow + told;    
            }        
            told = tnow;

            /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
            if (tcnt >= ticks)
            {
            	break;
            }
        }  
    }
} 

/*
*********************************************************************************************************
*	�� �� ��: bsp_GetRunTime
*	����˵��: ��ȡCPU����ʱ�䣬��λ1ms������Ա�ʾ 24.85�죬�����Ĳ�Ʒ��������ʱ�䳬�������������뿼���������
*	��    ��:  ��
*	�� �� ֵ: CPU����ʱ�䣬��λ1ms
*********************************************************************************************************
*/
uint32_t bsp_GetRunTime(void)
{
	uint32_t runtime;

	DISABLE_INT();  	/* ���ж� */

	runtime = Sys_Time;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */

	ENABLE_INT();  		/* ���ж� */

	return runtime;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitTimer
*	����˵��: ����systick�жϣ�����ʼ��������ʱ������
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitTimer(void)
{
	/*
		����systic�ж�����Ϊ1ms��������systick�жϡ�
    	SysTick_Config() �������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��Systick��ʱ�ж�.
	    	-- SystemCoreClock / 1000  ��ʾ��ʱƵ��Ϊ 1000Hz�� Ҳ���Ƕ�ʱ����Ϊ  1ms
	    	-- SystemCoreClock / 500   ��ʾ��ʱƵ��Ϊ 500Hz��  Ҳ���Ƕ�ʱ����Ϊ  2ms
	    	-- SystemCoreClock / 2000  ��ʾ��ʱƵ��Ϊ 2000Hz�� Ҳ���Ƕ�ʱ����Ϊ  500us

    	���ڳ����Ӧ�ã�����һ��ȡ��ʱ����1ms�����ڵ���CPU���ߵ͹���Ӧ�ã��������ö�ʱ����Ϊ 10ms
    */
	SysTick_Config(SystemCoreClock / 1000);
}

/*
*********************************************************************************************************
*	�� �� ��: SysTick_ISR
*	����˵��: SysTick�жϷ������ÿ��1ms����1��
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
	Sys_Time++;
	/* ÿ��1ms����1�� �������� bsp_DelayMS�� */
	if (DelayCount > 0)
	{
		if (--DelayCount == 0)
		{
			TimeOutFlag = 1;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: Ceshi_Task
*	����˵��: ���Ժ���ʵ��������ִ�в�����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Ceshi_Task0(void)
{
    static uint32_t sys_time, time_ms = 0, time_add_up = 0;
	
	DISABLE_INT();  	/* ���ж� */
	sys_time = Sys_Time;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */
	ENABLE_INT();  		/* ���ж� */
	
	if(time_ms != sys_time)
	{
		time_ms = sys_time;
		if(++time_add_up >= 1000)
		{
				time_add_up = 0;
				
		}
	}
}

void Ceshi_Task1(void)
{
  static uint32_t sys_time, time_ms = 0, time_add_up = 0, work_time_max = 1000;
	uint32_t offset_time_ticks = 0;
	uint8_t timeout_flag = 0;
	
	uint32_t err;
	uint32_t iTime1, iTime2;
	
	DISABLE_INT();  	/* ���ж� */
	sys_time = Sys_Time;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */
	ENABLE_INT();  		/* ���ж� */
	
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
			err = err;
			iTime1 = bsp_GetRunTime();
			iTime2 = bsp_GetRunTime();
			printf("\r\ntime = %dms\r\n", iTime2 - iTime1);
		}
	}
}