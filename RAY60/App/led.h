#ifndef _LED_H_
#define _LED_H_

#include "sys_config.h"  

typedef enum 
{
	fault,success
}OPERATE_RESULT;

void LED_BUZZER_CMD(OPERATE_RESULT datain);
void LED_BUZZER_Run(void);

#endif