#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#include "stm32f10x.h"  
#include "system_stm32f10x.h"

typedef uint8_t     BYTE;  
typedef uint16_t    HALF;
typedef uint32_t    WORD;

#define SOF		0xaa
#define EOF		0x55
#define MARK	0xff

typedef enum 
{
	com_idle, 
	com_sof, 
	com_normal,
	com_mark,
	com_eof
}COM_SATA;


#include "sys_peripheral.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "key.h"
#include "led.h"

#include "communicate_to_rlm.h"

#endif