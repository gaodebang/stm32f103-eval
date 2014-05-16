#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "sys_config.h"  

#ifdef		DISPLAY_GLOBALS
#define   DISPLAY_EXT
#else 
#define		DISPLAY_EXT		extern 
#endif

DISPLAY_EXT u8 QR_Bmp_Data[50][50];

void Display_wait(u8 datain);
void DispInit(void);
void DisplayBattery(void);
void Display_Wine_Message(u8 * datain);
void Display_User_Data(u8 *datain,u8 blank);
void Display_Sn_Data(u8 * datain);
void Display_Serianet_Mark(void);
void Display_Error(void);
void Display_Welcome(void);
void Display_Sn_Qr(void);
#endif


