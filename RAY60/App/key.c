#define		KEY_GLOBALS
#include "key.h"

static u8 KEY_CON;

static void get_key_data(void)
{
	u8 key_now_stat;
	static u8 key_temp_stat=0;
	static u8 key_time;
#ifdef NEW_BOARD
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)==RESET)
#else
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)==RESET)
#endif
	{
		key_now_stat=0;
	}
	else
	{
		key_now_stat=1;
	}
	
	if((key_now_stat==0)&&(key_temp_stat==0))
	{
		key_temp_stat=1;
		key_time=20;
	}

	if((key_now_stat==0)&&(key_temp_stat==1))
	{	
		//此按键程序按键按下20ms后即触发按键处理程序
		if(key_time>1)
		{
			key_time--;
		}
		else if(key_time==1)
		{
			KEY_CON++;//表明有一次有效按键
			key_time=0;
		}
	}
	if(key_now_stat==1)
	{
		key_temp_stat=0;
	}
}

static void key_operate_deal(void)
{
  static u8 key_con;
  if(key_con!=KEY_CON)
	{
		key_con=KEY_CON;
		App_Start();
	}
}

void KEY_Run(void)
{
	u8 sys_time_ms;
	static u8 time_ms = 0;
	
	__disable_irq();
	sys_time_ms = SYS_TIME_MS;
	__enable_irq();	
	if(time_ms != sys_time_ms)
	{
		time_ms = sys_time_ms;
		get_key_data();
		key_operate_deal();
	}	
}