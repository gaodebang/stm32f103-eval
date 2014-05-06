/* Includes ------------------------------------------------------------------*/
#include "sys_config.h"

int main(void)
{
  Set_System();                     //系统初始化
  while (1)
  {
		USART1_Run();										//串口1实时运行函数
		USART2_Run();										//串口2实时运行函数
		USART3_Run();										//串口3实时运行函数
		KEY_Run();											//按键实时监测函数
		LED_BUZZER_Run();								//led、buzzer 实时状态更新函数
		COMMUNICATE_TO_RLM_Run();				//通信处理函数
		WWDG_Reload_Task();							//窗口看门狗喂狗
		IWDG_ReloadCounter();						//独立看门狗喂狗
  }
}
