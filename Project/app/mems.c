/*
*********************************************************************************************************
*
*	模块名称 : MEMS传感器模块(For STM32F4XX)
*	文件名称 : mems.c
*	版    本 : V1.0
*	说    明 : 
*
*********************************************************************************************************
*/

#include "app.h"


/*
*********************************************************************************************************
*	函 数 名: Mems_Task
*	功能说明: 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void mems_updata(void)
{
    static uint8_t time_mark = 0;
    if(++time_mark >= 3)
    {
        time_mark = 0;
    }
    if(time_mark == 0)
    {
        BH1750_GetLux();
    }
    else if(time_mark == 1)
    {
        MPU6050_ReadData();
    }
    else if(time_mark == 2)
    {
        HMC5883L_ReadData();
    }
}

/*
*********************************************************************************************************
*	函 数 名: Mems_Task
*	功能说明: 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void Mems_Task(void)
{
    static uint32_t sys_time, time_ms = 0, time_add_up = 0, work_time_max = 30;
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
			mems_updata();
		}
    }
}

