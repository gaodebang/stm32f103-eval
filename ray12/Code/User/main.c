/* Includes ------------------------------------------------------------------*/
#include "sys_config.h"

int main(void)
{
  Set_System();                     //系统初始化
  while (1)
  {
		USART1_Run();										//串口1实时运行函数
		USART2_Run();										//串口2实时运行函数
		KEY_Run();											//按键实时监测函数
		COMMUNICATE_TO_RLM_Run();				//通信处理函数
  }
}
