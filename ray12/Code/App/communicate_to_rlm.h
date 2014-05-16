#ifndef _COMMUNICATE_TO_RLM_H_
#define _COMMUNICATE_TO_RLM_H_

#include "sys_config.h"

#ifdef		COMMUNICATE_GLOBALS
#define   COMMUNICATE_EXT
#else 
#define		COMMUNICATE_EXT		extern 
#endif

COMMUNICATE_EXT u8 Error_Times_Mark;
COMMUNICATE_EXT u8 * jiujingdu_addr;
typedef enum 
{
	sys_idle_state,
	sys_updata_state,
	normal_work_state,
	passthrough_state,
	on_line_work_state
}SYS_WORK_STATE;

typedef enum 
{
	rmu_idle_mode,
	rmu_wait_rlm_wakeup_mode,
	rmu_wait_rlm_user_data_mode,
	rmu_wait_rlm_tid_data_mode,
	rmu_dis_sn_mode,
	rmu_dis_qr_mode
}WORK_COM_STATE;

/*系统当前模式*/
COMMUNICATE_EXT SYS_WORK_STATE 						WORK_STATE;
/*正常工作时运行状态*/
COMMUNICATE_EXT WORK_COM_STATE						NORMAL_WORK_COM_STATE;

void App_Start(void);
void COMMUNICATE_Variable_Init(void);
void COMMUNICATE_TO_RLM_Run(void);
void Aes_Deal(u8 *data_temp_sn);
#endif
