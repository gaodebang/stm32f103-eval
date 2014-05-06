/*
*********************************************************************************************************
*
*	ģ������ : CAMERAģ��(For STM32F4XX)
*	�ļ����� : camera.c
*	��    �� : V1.0
*	˵    �� : 
*
*********************************************************************************************************
*/

#include "app.h"

/*
*********************************************************************************************************
*	�� �� ��: Camera_Task
*	����˵��: 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Camera_Task(void)
{
    static uint32_t sys_time, time_ms = 0, time_add_up = 0, work_time_max = 300;
    static uint8_t first_times_mark = 0;
	uint32_t offset_time_ticks = 0;
	uint8_t timeout_flag = 0;
	
	DISABLE_INT();  	/* ���ж� */
	if(first_times_mark == 0)
	{
	    sys_time = Sys_Time;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */
	    first_times_mark = 1;
	    time_ms = sys_time;
    }
    else
    {
        sys_time = Sys_Time;	/* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */
    }
	ENABLE_INT();  		/* ���ж� */
	
	if(time_ms != sys_time)
    {
        if(time_ms < sys_time)
    	{
    	    offset_time_ticks = sys_time - time_ms;
    		time_ms = sys_time;
    		if((offset_time_ticks  + time_add_up) > UINT_LEAST32_MAX)
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
    	    if((offset_time_ticks  + time_add_up) > UINT_LEAST32_MAX)
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
			if(Pic_Dis_Mark == 0)
            {			    
                CAM_Start(EXT_SRAM_ADDR);
            }
		}
    }
}

