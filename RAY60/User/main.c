/* Includes ------------------------------------------------------------------*/
#include "sys_config.h"

int main(void)
{
  Set_System();                     //ϵͳ��ʼ��
  while (1)
  {
		USART1_Run();										//����1ʵʱ���к���
		USART2_Run();										//����2ʵʱ���к���
		USART3_Run();										//����3ʵʱ���к���
		KEY_Run();											//����ʵʱ��⺯��
		LED_BUZZER_Run();								//led��buzzer ʵʱ״̬���º���
		COMMUNICATE_TO_RLM_Run();				//ͨ�Ŵ�������
		WWDG_Reload_Task();							//���ڿ��Ź�ι��
		IWDG_ReloadCounter();						//�������Ź�ι��
  }
}