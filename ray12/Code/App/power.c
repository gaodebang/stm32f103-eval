#define POWER_GLOBALS
#include "power.h"

//�ر���Ļ������͹���ģʽ
void EnterStandby(void)
{
    GPIO_SetBits(LCD_PWR_Port, LCD_PWR_Pin);   //�ر���Ļ
   
    //ʹ��WAKE-UP�ܽ�
    PWR_WakeUpPinCmd(ENABLE);
    
    PWR_EnterSTANDBYMode(); //����standbyģʽ
}

