#define POWER_GLOBALS
#include "power.h"

//关闭屏幕，进入低功耗模式
void EnterStandby(void)
{
    GPIO_SetBits(LCD_PWR_Port, LCD_PWR_Pin);   //关闭屏幕
   
    //使能WAKE-UP管脚
    PWR_WakeUpPinCmd(ENABLE);
    
    PWR_EnterSTANDBYMode(); //进入standby模式
}

