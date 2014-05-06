#ifndef _KEY_H_
#define _KEY_H_

#include "sys_config.h"  

#ifdef		KEY_GLOBALS
#define   KEY_EXT
#else 
#define		KEY_EXT		extern 
#endif



void KEY_Run(void);

#endif