#ifndef _COMMUNICATE_TO_RLM_H_
#define _COMMUNICATE_TO_RLM_H_

#include "sys_config.h"

#ifdef		COMMUNICATE_GLOBALS
#define   COMMUNICATE_EXT
#else 
#define		COMMUNICATE_EXT		extern 
#endif

typedef enum 
{
	sys_idle_state, 
	sys_updata_state,
	normal_work_state,
	passthrough_state,
	shake_hands_state
}SYS_WORK_STATE;

typedef enum 
{
	com_by_eth,
	com_by_com
}COM_PORT;

typedef enum 
{
	rmu_idle_mode,
	rmu_wait_rlm_wakeup_mode,
	rmu_wait_rlm_reserved_data_mode,
	rmu_wait_rlm_uii_data_mode,
	rmu_wait_rlm_tid_data_mode,
	rmu_wait_rlm_user_data_mode,
	rmu_wait_pc_data_mode,
	rmu_wait_rlm_reserved_ack_mode,
	rmu_wait_rlm_user_ack_mode,
	rmu_wait_rlm_lock_ack_mode
}WORK_COM_STATE;

typedef enum 
{
	sys_idle_mode,
	sys_wait_start_cmd_mode,
	sys_wait_shake_hands_end_mode
}UPDATA_COM_STATE;

/*ϵͳ��ǰģʽ*/
COMMUNICATE_EXT SYS_WORK_STATE 						WORK_STATE;
/*ѡ��������ʱʱ���ͨ�Žӿ�*/
COMMUNICATE_EXT COM_PORT 									WORK_COM_PORT;
/*��������ʱ����״̬*/
COMMUNICATE_EXT WORK_COM_STATE						NORMAL_WORK_COM_STATE;
/*��������ʱ����״̬*/
COMMUNICATE_EXT UPDATA_COM_STATE					UPDATA_WORK_COM_STATE;
void App_Start(void);
void COMMUNICATE_Variable_Init(void);
void COMMUNICATE_TO_RLM_Run(void);

#endif