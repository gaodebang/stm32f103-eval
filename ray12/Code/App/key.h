#ifndef _KEY_H_
#define _KEY_H_

#include "sys_config.h"  

#ifdef		KEY_GLOBALS
#define   KEY_EXT
#else 
#define		KEY_EXT		extern 
#endif

KEY_EXT u8 KEY_DATA;
KEY_EXT u8 KEY_CON;

void KEY_Variable_Init(void);
void KEY_Run(void);

#endif