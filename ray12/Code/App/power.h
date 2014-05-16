#ifndef _POWER_H_
#define _POWER_H_

#include "sys_config.h"  

#ifdef		POWER_GLOBALS
#define   POWER_EXT
#else 
#define		POWER_EXT		extern 
#endif

void EnterStandby(void);

#endif