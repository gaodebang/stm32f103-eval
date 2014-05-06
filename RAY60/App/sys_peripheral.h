#ifndef _SYS_PERIPHERAL_C_
#define _SYS_PERIPHERAL_C_

#include "sys_config.h"

#ifdef		PERIPHERAL_GLOBALS
#define   PERIPHERAL_EXT
#else 
#define		PERIPHERAL_EXT		extern 
#endif

#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)
#define WWDG_COUNTER          					(0X7F)

// BAUD
#define BAUD_RATE_9600                  (0x00)
#define BAUD_RATE_19200                 (0x01)
#define BAUD_RATE_57600                 (0x02)
#define BAUD_RATE_115200                (0x03)


PERIPHERAL_EXT vu8 SYS_TIME_MS;


void Delay_Ms(u32 nTime); 
void Timingdelay_Decrement(void);
void Reset_Wwdg();
void WWDG_Reload_Task(void);
void NVIC_GenerateSystemReset(void);
void Set_System(void);

#endif

