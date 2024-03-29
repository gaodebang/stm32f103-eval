#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#include "stm32f10x.h"  
#include "stm32f10x_it.h"  
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

typedef enum {FALSE = 0, TRUE = !RESET} BOOL;

#include "sys_peripheral.h"
#include "usart1.h"
#include "usart2.h"
#include "rtc.h"
#include "power.h"

#include "EmAES.h"
#include "key.h"
#include "GUI.h"
#include "ssd1332_lcd.h"
#include "display.h"
#include "QR_Encode.h"

#include "communicate_to_rlm.h"

#endif

/*
GUI_BLACK  						0x000000       ��  	 
GUI_BLUE  						0xFF0000       ��  	 
GUI_GREEN  						0x00FF00       ��  	 
GUI_CYAN  						0xFFFF00       ��  	 
GUI_RED  							0x0000FF       ��  	 
GUI_MAGENTA  					0x8B008B       ���  
GUI_BROWN  					  0x2A2AA5       ��  	 
GUI_DARKGRAY  				0x404040       ���  
GUI_GRAY  						0x808080       ��  	 
GUI_LIGHTGRAY  				0xD3D3D3       ǳ��  
GUI_LIGHTBLUE  				0xFF8080       ����  
GUI_LIGHTGREEN  			0x80FF80       ����  
GUI_LIGHTCYAN  				0x80FFFF       ����  
GUI_LIGHTRED  				0x8080FF       ����  
GUI_LIGHTMAGENTA    	0xFF80FF       �����
GUI_YELLOW  					0x00FFFF       ��  	 
GUI_WHITE  						0xFFFFFF       ��  	 
*/