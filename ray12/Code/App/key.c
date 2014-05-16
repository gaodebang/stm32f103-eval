 #define		KEY_GLOBALS
#include "key.h"

static void get_key_data(void)
{
	u8 key0_now_stat;
	u8 key1_now_stat;
	u8 key2_now_stat;
	u8 key3_now_stat;
	
	static u8 key0_temp_stat=0;
	static u8 key1_temp_stat=0;
	static u8 key2_temp_stat=0;
	static u8 key3_temp_stat=0;
	
	static u16 key0_time=0;
	static u16 key1_time=0;
	static u16 key2_time=0;
	static u16 key3_time=0;
	
	switch(GPIO_ReadInputData(GPIOA)&0x1901)
	{
		case 0x0001 :
			key0_now_stat=1;
			key1_now_stat=0;
			key2_now_stat=0;
			key3_now_stat=0;
		break;
		case 0x0100 :
			key0_now_stat=0;
			key1_now_stat=1;
			key2_now_stat=0;
			key3_now_stat=0;
		break;		
		case 0x0800 :
			key0_now_stat=0;
			key1_now_stat=0;
			key2_now_stat=1;
			key3_now_stat=0;
		break;		
		case 0x1000 :
			key0_now_stat=0;
			key1_now_stat=0;
			key2_now_stat=0;
			key3_now_stat=1;
		break;
		default:
			key0_now_stat=0;
			key1_now_stat=0;
			key2_now_stat=0;
			key3_now_stat=0;
		break;
	}
	
	if((key0_now_stat==1)&&(key0_temp_stat==0))
	{
		key0_temp_stat=1;
		key0_time=2000;
	}
	if((key1_now_stat==1)&&(key1_temp_stat==0))
	{
		key1_temp_stat=1;
		key1_time=20;
	}
	if((key2_now_stat==1)&&(key2_temp_stat==0))
	{
		key2_temp_stat=1;
		key2_time=20;
	}
	if((key3_now_stat==1)&&(key3_temp_stat==0))
	{
		key3_temp_stat=1;
		key3_time=20;
	}
	
	if((key0_now_stat==1)&&(key0_temp_stat==1))
	{	
		//此按键程序按键按下20ms后即触发按键处理程序
		if(key0_time>1)
		{
			key0_time--;
		}
		else if(key0_time==1)
		{
			KEY_CON++;//表明有一次有效按键
			KEY_DATA=0;
			key0_time=0;
			EnterStandby();
		}
	}
	if((key1_now_stat==1)&&(key1_temp_stat==1))
	{
		//此按键程序按键按下20ms后即触发按键处理程序
		if(key1_time>1)
		{
			key1_time--;
		}
		else if(key1_time==1)
		{
			KEY_CON++;//表明有一次有效按键
			KEY_DATA=1;
			key1_time=0;
		}
	}
	if((key2_now_stat==1)&&(key2_temp_stat==1))
	{	
		//此按键程序按键按下20ms后即触发按键处理程序
		if(key2_time>1)
		{
			key2_time--;
		}
		else if(key2_time==1)
		{
			KEY_CON++;//表明有一次有效按键
			KEY_DATA=2;			
			key2_time=0;
		}
	}
	if((key3_now_stat==1)&&(key3_temp_stat==1))
	{	
		//此按键程序按键按下20ms后即触发按键处理程序
		if(key3_time>1)
		{
			key3_time--;
		}
		else if(key3_time==1)
		{
			KEY_CON++;//表明有一次有效按键
			KEY_DATA=3;				
			key3_time=0;
		}
	}
	
	if(key0_now_stat==0)
	{
		key0_temp_stat=0;
	}
	if(key1_now_stat==0)
	{
		key1_temp_stat=0;
	}
	if(key2_now_stat==0)
	{
		key2_temp_stat=0;
	}
	if(key3_now_stat==0)
	{
		key3_temp_stat=0;
	}
}

static void key_operate_deal(void)
{
  static u8 key_con=0;
  if(key_con!=KEY_CON)
	{
		key_con=KEY_CON;
		clear_guanji_counter();
		DisplayBattery();
		switch(KEY_DATA)
		{
			case 1:
				if(WORK_STATE==sys_idle_state)
				{
					Display_wait(0);
					WORK_STATE=normal_work_state;
					App_Start();						
				}
				break;
			case 2:
				if(WORK_STATE==sys_idle_state)
				{
					if(NORMAL_WORK_COM_STATE==rmu_dis_sn_mode)
					{
						Aes_Deal(jiujingdu_addr);
						NORMAL_WORK_COM_STATE=rmu_dis_qr_mode;
					}
					else if(NORMAL_WORK_COM_STATE==rmu_dis_qr_mode)
					{
						Display_Sn_Data(jiujingdu_addr);
						NORMAL_WORK_COM_STATE=rmu_dis_sn_mode;
					}
					else
					{
						Display_wait(1);
						WORK_STATE=on_line_work_state;
						App_Start();
					}
				}
				break;
			case 3:
				if(WORK_STATE!=passthrough_state)
				{
					WORK_STATE=sys_idle_state;
					NORMAL_WORK_COM_STATE=rmu_idle_mode;
					Display_Welcome();
				}
				break;
			default :
				break;
		}
	}        
}          
void KEY_Variable_Init(void)
{          
	KEY_CON=0;
	KEY_DATA=0;
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
