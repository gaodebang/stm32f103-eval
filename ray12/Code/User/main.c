/* Includes ------------------------------------------------------------------*/
#include "sys_config.h"

int main(void)
{
  Set_System();                     //ϵͳ��ʼ��
  while (1)
  {
		USART1_Run();										//����1ʵʱ���к���
		USART2_Run();										//����2ʵʱ���к���
		KEY_Run();											//����ʵʱ��⺯��
		COMMUNICATE_TO_RLM_Run();				//ͨ�Ŵ�������
  }
}