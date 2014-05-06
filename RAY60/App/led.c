#include "led.h"
#ifdef NEW_BOARD
	#define led_buzzer_on 	(GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_SET))
	#define led_buzzer_off 	(GPIO_WriteBit(GPIOA, GPIO_Pin_6, Bit_RESET))
#else
	#define led_buzzer_on 	(GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET))
	#define led_buzzer_off 	(GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET))
#endif
static u16 On_Time;

static void led_buzzer_deal(void)
{
	if(On_Time!=0)
	{
		On_Time--;
		if(On_Time==0)
		{
			led_buzzer_off;
		}
	}
}

void LED_BUZZER_CMD(OPERATE_RESULT datain)
{
	led_buzzer_on;
	switch(datain)
	{
		case success:
			On_Time=200;
			break;
		case fault	:
			On_Time=1000;
			break;
		default:
			break;
	}
}
void LED_BUZZER_Run(void)
{
	u8 sys_time_ms;
	static u8 time_ms = 0;
	
	__disable_irq();
	sys_time_ms = SYS_TIME_MS;
	__enable_irq();	
	if(time_ms != sys_time_ms)
	{
		time_ms = sys_time_ms;
		led_buzzer_deal();
	}	
}