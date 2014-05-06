#define		COMMUNICATE_GLOBALS
#include "communicate_to_rlm.h"

#define Time_Out_Reload_A (2000)
#define Time_Out_Reload_B (100)

static uc8 rlm_wakeup_data[]						=	{0x02,0x12};
static uc8 rlm_get_user_data[]					=	{0x09,0x20,0x00,0x00,0x00,0x00,\
																					 0x03,0,10};
static uc8 rlm_lock_rfid_data[]					=	{0x0b,0x25,0x00,0x00,0x00,0x00,\
																					 3,0x20,1,\
																					 0xff,0xff};

static uc8 send_pc_success_ack[]				=	{0x03,0x1e,0x00};
static uc8 send_pc_fail_ack[]						=	{0x03,0x1e,0x80};

static u8 rlm_send_pc_data	 		 [40]   = {0x28,0x0e,\
																					 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
																					 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

static u8 rlm_write_reserved_data[25]		=	{0x19,0x25,0x00,0x00,0x00,0x00,\
																					 3,10,8,\
																					 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static u8 rlm_write_user_data		 [25]		=	{0x19,0x25,0x00,0x00,0x00,0x00,\
																					 3,10,8,\
																					 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

static u32 Time_Out=0;

static u16 shake_hands_time_add=0;
/*==================================================================================
* �� �� ���� shake_hands_start
* ��    ���� None
* ��������:  ����һ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void shake_hands_start(void)
{	
	u8 shake_hands_data[]={0x03,0x0d,0x00};
	Time_Out=Time_Out_Reload_B;
	WORK_STATE=shake_hands_state;
	if(WORK_COM_PORT == com_by_com)
	{
		USART1_Tx_Chars(shake_hands_data,3);
	}
	else
	{
		USART2_Tx_Chars(shake_hands_data,3);
	}
}

/*==================================================================================
* �� �� ���� crccalc
* ��    ���� u8* pTagFrame,u16 len,u8* pTagCrc
* ��������:  CRCУ��ֵ���㷵�غ���
* �� �� ֵ�� None
* ��    ע�� CRCУ��ֵ�ķ���ͨ��ָ��
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void crccalc(u8* pTagFrame,u16 len,u8* pTagCrc)
{
    u8 i, j;
    u16 XorVal;
    u8 crcHigh, crcLow;
    u16 CrcResult = 0xFFFF;

    for(i = 0; i < len; i++, pTagFrame++)
    {
			for(j = 0; j < 8; j++)
			{
			    XorVal = ((CrcResult>>8)^(*pTagFrame<<j)) & 0x0080;
			    CrcResult = (CrcResult<<1) & 0xfffe;
			    if(XorVal)
			    {CrcResult ^= 0x1021;}
			}		
    }

    CrcResult = ~CrcResult;

    crcHigh = (BYTE)(CrcResult >> 8);
    CrcResult <<= 8;
    crcLow = (BYTE)(CrcResult >> 8);
		*pTagCrc = crcHigh;
    *(pTagCrc + 1) = crcLow;
}
/*==================================================================================
* �� �� ���� deal_write_epc_data
* ��    ���� None
* ��������:  �����ȡ�ı�ǩ���ݣ����ϣ������Ժ��͸���λ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void deal_write_epc_data(u8 * data_addr)
{
	u8 i;
	*(rlm_write_reserved_data+6)=0;
	*(rlm_write_reserved_data+7)=4;
	*(rlm_write_reserved_data+8)=8;
	
	for(i=0;i<16;i++)
	{
		*(rlm_write_reserved_data+9+i)=*(data_addr+i);
	}

	*(rlm_write_user_data+6)=3;
	*(rlm_write_user_data+7)=10;
	*(rlm_write_user_data+8)=8;
	for(i=0;i<16;i++)
	{
		*(rlm_write_user_data+9+i)=*(data_addr+16+i);
	}
}

/*==================================================================================
* �� �� ���� deal_send_pc_data
* ��    ���� None
* ��������:  �����ȡ�ı�ǩ���ݣ����ϣ������Ժ��͸���λ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void deal_send_pc_data(u8 *data_addr)
{
	u8 i;
	for(i=0;i<38;i++)
	{
		*(rlm_send_pc_data+2+i)=data_addr[i];
	}
}

/*==================================================================================
* �� �� ���� deal_usart1_rxd_in_idle
* ��    ���� None
* ��������:  ϵͳ����ʱ����1�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart1_rxd_in_idle(void)
{
	u8 set_rmu_cmd_ack[]	=	{0x03,0x00,0x00};
	u16 data_len;
	u8 cmd_temp;
	u8 recrc[2];
	if(*(USART1_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f)*128+(u16)(*(USART1_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+2);
	}
	else
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+1);
	}
	set_rmu_cmd_ack[1]=cmd_temp;
	if(cmd_temp&0x80)
	{
		crccalc(USART1_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART1_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART1_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}

	if(((cmd_temp&0x7f)==0x0B)&&(data_len==0x02))//�л���com��rlm͸��ģʽ
	{
		WORK_COM_PORT = com_by_com;
		WORK_STATE=passthrough_state;
	}
	else if(((cmd_temp&0x7f)==0x1B)&&(data_len==0x02))
	{
		WORK_COM_PORT = com_by_com;
	}
	else if(((cmd_temp&0x7f)==0x0C)&&(data_len==0x02))
	{
		WORK_COM_PORT = com_by_eth;
	}
	else if(((cmd_temp&0x7f)==0x1C)&&(data_len==0x02))
	{
		WORK_COM_PORT = com_by_com;
	}
	else if(((cmd_temp&0x7f)==0x1D)&&(data_len==0x02))
	{
		shake_hands_start();
		return 0;
	}
	else if(((cmd_temp&0x7f)==0x0F)&&(data_len==0x02))
	{
		WORK_COM_PORT = com_by_com;
		App_Start();
		return 0;
	}
	else if(((cmd_temp&0x7f)==0x71)&&(data_len==0x06))
	{
		if((*(USART1_RXD_DATA+2)==0xA0)&&(*(USART1_RXD_DATA+3)==0xA0)&&(*(USART1_RXD_DATA+4)==0x50)&&(*(USART1_RXD_DATA+5)==0x50))
		{
			WORK_STATE=sys_updata_state;
			UPDATA_WORK_COM_STATE=sys_wait_shake_hands_end_mode;
		}
		else
		{
			set_rmu_cmd_ack[2]=0x80;
		}
	}
	else
	{
		set_rmu_cmd_ack[2]=0x80;
	}
	USART1_Tx_Chars(set_rmu_cmd_ack,3);
	return 0;
}

/*==================================================================================
* �� �� ���� deal_usart1_rxd_in_normal_work
* ��    ���� None
* ��������:  ϵͳ����ģʽʱ����1�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart1_rxd_in_normal_work(void)
{
	u16 data_len;
	u8 cmd_temp;	
	u8 cmd_stat;
	u8 recrc[2];	
	u8 * data_satrt_addr;
	if(*(USART1_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f)*128+(u16)(*(USART1_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+2);
		cmd_stat=*(USART1_RXD_DATA+3);
		data_satrt_addr=USART1_RXD_DATA+4;
	}
	else
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+1);
		cmd_stat=*(USART1_RXD_DATA+2);
		data_satrt_addr=USART1_RXD_DATA+3;
	}
	if(cmd_temp&0x80)
	{
		crccalc(USART1_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART1_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART1_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}
	switch(NORMAL_WORK_COM_STATE)
	{
		case rmu_wait_pc_data_mode:
			if((cmd_stat==0)&&(cmd_temp==0x0e)&&(data_len==0x23))
			{
				deal_write_epc_data(data_satrt_addr);
				USART3_Tx_Chars(rlm_write_reserved_data,25);
				NORMAL_WORK_COM_STATE	=rmu_wait_rlm_reserved_ack_mode;
			}
			else
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ
			}
			break;
		default:
			break;
	}
	return 0;
}

/*==================================================================================
* �� �� ���� deal_usart1_rxd_in_passthrough
* ��    ���� None
* ��������:  ϵͳ����ʱ����1�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart1_rxd_in_passthrough(void)
{
	u8 set_rmu_cmd_ack[]	=	{0x03,0x00,0x00};
	u16 data_len;
	u8 cmd_temp;
	u8 recrc[2];
	if(*(USART1_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f)*128+(u16)(*(USART1_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+2);
	}
	else
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+1);
	}
	set_rmu_cmd_ack[1]=cmd_temp;
	if(cmd_temp&0x80)
	{
		crccalc(USART1_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART1_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART1_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}
	
	if((cmd_temp==0x0b)||(cmd_temp==0x1b)||(cmd_temp==0x0c)||(cmd_temp==0x1c)||(cmd_temp==0x0d)||(cmd_temp==0x1d)||(cmd_temp==0x0e)||(cmd_temp==0x1e)||(cmd_temp==0x0f))
	{
		if(((cmd_temp&0x7f)==0x1B)&&(data_len==0x02))
		{
			WORK_COM_PORT=com_by_com;
			WORK_STATE=sys_idle_state;
			USART1_Tx_Chars(set_rmu_cmd_ack,3);
		}
		else if(((cmd_temp&0x7f)==0x0B)&&(data_len==0x02))
		{
			WORK_COM_PORT=com_by_com;
			USART1_Tx_Chars(set_rmu_cmd_ack,3);
		}
		else
		{
			*(set_rmu_cmd_ack+2)=0x80;
			USART1_Tx_Chars(set_rmu_cmd_ack,3);
		}
	}
	else if(WORK_COM_PORT==com_by_com)
	{
		USART3_Tx_Chars(USART1_RXD_DATA,USART1_RXD_CON);
	}		
	return 0;
}

static u8 deal_usart1_rxd_in_shake_hands(void)
{
	u16 data_len;
	u8 cmd_temp;	
	u8 cmd_stat;
	u8 recrc[2];	
	if(*(USART1_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f)*128+(u16)(*(USART1_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+2);
		cmd_stat=*(USART1_RXD_DATA+3);
	}
	else
	{
		data_len=(u16)(*(USART1_RXD_DATA)&0x7f);
		cmd_temp=*(USART1_RXD_DATA+1);
		cmd_stat=*(USART1_RXD_DATA+2);
	}
	if(cmd_temp&0x80)
	{
		crccalc(USART1_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART1_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART1_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}
	if((cmd_temp==0x0d)&&(data_len==0x03))
	{
		if(cmd_stat==0)
		{
			WORK_STATE=sys_idle_state;
			NORMAL_WORK_COM_STATE=rmu_idle_mode;	
		}
		else
		{
			LED_BUZZER_CMD(fault);
			WORK_STATE=sys_idle_state;
			NORMAL_WORK_COM_STATE=rmu_idle_mode;
		}
	}
	return 0;
}
/*==================================================================================
* �� �� ���� deal_usart2_rxd_in_idle
* ��    ���� None
* ��������:  ϵͳ����ʱ����1�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart2_rxd_in_idle(void)
{
	u8 set_rmu_cmd_ack[]	=	{0x03,0x00,0x00};
	u16 data_len;
	u8 cmd_temp;
	u8 recrc[2];
	if(*(USART2_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f)*128+(u16)(*(USART2_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+2);
	}
	else
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+1);
	}
	set_rmu_cmd_ack[1]=cmd_temp;
	if(cmd_temp&0x80)
	{
		crccalc(USART2_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART2_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART2_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}

	if(((cmd_temp&0x7f)==0x0B)&&(data_len==0x02))//�л���com��rlm͸��ģʽ
	{
		WORK_COM_PORT=com_by_eth;
		WORK_STATE=passthrough_state;
	}
	else if(((cmd_temp&0x7f)==0x1B)&&(data_len==0x02))
	{
		WORK_COM_PORT = com_by_eth;
	}
	else if(((cmd_temp&0x7f)==0x1D)&&(data_len==0x02))
	{
		shake_hands_start();
		return 0;
	}
	else
	{
		set_rmu_cmd_ack[2]=0x80;
	}	
	USART2_Tx_Chars(set_rmu_cmd_ack,3);
	return 0;
}

/*==================================================================================
* �� �� ���� deal_usart2_rxd_in_master
* ��    ���� None
* ��������:  ϵͳ����ģʽʱ����2�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart2_rxd_in_normal_work(void)
{
	u16 data_len;
	u8 cmd_temp;
	u8 cmd_stat;
	u8 recrc[2];
	u8 * data_satrt_addr;
	if(*(USART2_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f)*128+(u16)(*(USART2_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+2);
		cmd_stat=*(USART2_RXD_DATA+3);
		data_satrt_addr=USART2_RXD_DATA+4;
	}
	else
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+1);	
		cmd_stat=*(USART2_RXD_DATA+2);
		data_satrt_addr=USART2_RXD_DATA+3;
	}
	if(cmd_temp&0x80)
	{
		crccalc(USART2_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART2_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART2_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}
	switch(NORMAL_WORK_COM_STATE)
	{
		case rmu_wait_pc_data_mode:
			if((cmd_stat==0)&&(cmd_temp==0x0e)&&(data_len==0x23))
			{
				deal_write_epc_data(data_satrt_addr);
				USART3_Tx_Chars(rlm_write_reserved_data,25);
				NORMAL_WORK_COM_STATE	=rmu_wait_rlm_reserved_ack_mode;
			}
			else
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ					
			}
			break;
		default:
			break;
	}
	return 0;
}
/*==================================================================================
* �� �� ���� deal_usart2_rxd_in_passthrough
* ��    ���� None
* ��������:  ϵͳ����ʱ����1�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart2_rxd_in_passthrough(void)
{
	u8 set_rmu_cmd_ack[]	=	{0x03,0x00,0x00};
	u16 data_len;
	u8 cmd_temp;
	u8 recrc[2];
	if(*(USART2_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f)*128+(u16)(*(USART2_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+2);
	}
	else
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+1);
	}
	set_rmu_cmd_ack[1]=cmd_temp;
	if(cmd_temp&0x80)
	{
		crccalc(USART2_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART2_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART2_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}
	if((cmd_temp==0x0b)||(cmd_temp==0x1b)||(cmd_temp==0x0c)||(cmd_temp==0x1c)||(cmd_temp==0x0d)||(cmd_temp==0x1d)||(cmd_temp==0x0e)||(cmd_temp==0x1e)||(cmd_temp==0x0f))
	{
		if(((cmd_temp&0x7f)==0x1b)&&(data_len==0x02))
		{
			WORK_COM_PORT=com_by_eth;
			WORK_STATE=sys_idle_state;
			USART2_Tx_Chars(set_rmu_cmd_ack,3);
		}
		else if(((cmd_temp&0x7f)==0x0b)&&(data_len==0x02))
		{
			WORK_COM_PORT=com_by_eth;
			USART2_Tx_Chars(set_rmu_cmd_ack,3);
		}
		else
		{
			*(set_rmu_cmd_ack+2)=0x80;
			USART2_Tx_Chars(set_rmu_cmd_ack,3);
		}
	}
	else if(WORK_COM_PORT==com_by_eth)
	{
		USART3_Tx_Chars(USART2_RXD_DATA,USART2_RXD_CON);
	}
	return 0;
}


static u8 deal_usart2_rxd_in_shake_hands(void)
{
	u16 data_len;
	u8 cmd_temp;
	u8 cmd_stat;
	u8 recrc[2];
	if(*(USART2_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f)*128+(u16)(*(USART2_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+2);
		cmd_stat=*(USART2_RXD_DATA+3);
	}
	else
	{
		data_len=(u16)(*(USART2_RXD_DATA)&0x7f);
		cmd_temp=*(USART2_RXD_DATA+1);	
		cmd_stat=*(USART2_RXD_DATA+2);
	}
	if(cmd_temp&0x80)
	{
		crccalc(USART2_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART2_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART2_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}
	
	if((cmd_temp==0x0d)&&(data_len==0x03))
	{
		if(cmd_stat==0)
		{
			WORK_STATE=sys_idle_state;
			NORMAL_WORK_COM_STATE=rmu_idle_mode;	
		}
		else
		{
			LED_BUZZER_CMD(fault);
			WORK_STATE=sys_idle_state;
			NORMAL_WORK_COM_STATE=rmu_idle_mode;
		}
	}
	return 0;
}

/*==================================================================================
* �� �� ���� deal_usart3_rxd_in_normal_work
* ��    ���� None
* ��������:  ϵͳ����ģʽʱ����3�������������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static u8 deal_usart3_rxd_in_normal_work(void)
{
	u16 data_len;
	u8 cmd_temp;
	u8 cmd_stat;
	u8 recrc[2];
	u8 * data_satrt_addr;
	if(*(USART3_RXD_DATA)&0x80)
	{
		data_len=(u16)(*(USART3_RXD_DATA)&0x7f)*128+(u16)(*(USART3_RXD_DATA+1)&0x7f);
		cmd_temp=*(USART3_RXD_DATA+2);
		cmd_stat=*(USART3_RXD_DATA+3);
		data_satrt_addr=USART3_RXD_DATA+4;
	}
	else
	{
		data_len=(u16)(*(USART3_RXD_DATA)&0x7f);
		cmd_temp=*(USART3_RXD_DATA+1);
		cmd_stat=*(USART3_RXD_DATA+2);
		data_satrt_addr=USART3_RXD_DATA+3;
	}
	if(cmd_temp&0x80)
	{
		crccalc(USART3_RXD_DATA,data_len-2,recrc);
		if((*recrc!=*(USART3_RXD_DATA+data_len-2))||(*(recrc+1)!=*(USART3_RXD_DATA+data_len-1)))
		{
			return 0;
		}
	}

	switch(NORMAL_WORK_COM_STATE)
	{
			case rmu_wait_rlm_wakeup_mode:
			if((cmd_stat!=0)&&(cmd_temp==0x12))
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ
			}
			USART3_Tx_Chars((u8 *)rlm_get_user_data,9);
			NORMAL_WORK_COM_STATE=rmu_wait_rlm_user_data_mode;
			break;
		case rmu_wait_rlm_user_data_mode:
			if((cmd_stat!=0)&&(cmd_temp==0x20))
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ
			}
			deal_send_pc_data(data_satrt_addr);

			if(WORK_COM_PORT==com_by_eth)
			{
				USART2_Tx_Chars(rlm_send_pc_data,40);	
			}
			else if(WORK_COM_PORT==com_by_com)
			{
				USART1_Tx_Chars(rlm_send_pc_data,40);
			}
			NORMAL_WORK_COM_STATE=rmu_wait_pc_data_mode;
			break;				
		case rmu_wait_rlm_reserved_ack_mode:
			if((cmd_stat!=0)&&(cmd_temp==0x25))
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ
			}
			USART3_Tx_Chars(rlm_write_user_data,25);
			NORMAL_WORK_COM_STATE=rmu_wait_rlm_user_ack_mode;
			break;
		case rmu_wait_rlm_user_ack_mode:
			if((cmd_stat!=0)&&(cmd_temp==0x25))
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ
			}
			USART3_Tx_Chars((u8 *)rlm_wakeup_data,2);
			//USART3_Tx_Chars((u8 *)rlm_lock_rfid_data,13);
			NORMAL_WORK_COM_STATE=rmu_wait_rlm_lock_ack_mode;
			break;	
			
		case rmu_wait_rlm_lock_ack_mode:
			if((cmd_stat!=0)&&(cmd_temp==0x25))
			{
				return 0;//����ִ��ʧ�ܣ��������¿�ʼ
			}

			if(WORK_COM_PORT==com_by_eth)
			{USART2_Tx_Chars((u8 *)send_pc_success_ack,3);	}
			else if(WORK_COM_PORT==com_by_com)
			{USART1_Tx_Chars((u8 *)send_pc_success_ack,3);	}

			NORMAL_WORK_COM_STATE	=rmu_idle_mode;
			WORK_STATE=sys_idle_state;
			LED_BUZZER_CMD(success);
			break;
		default:
			break;
	}
	return 0;
}

/*==================================================================================
* �� �� ���� time_out_serve
* ��    ���� None
* ��������:  ͨ�ų�ʱ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void time_out_serve(void)
{
	u8 sys_time_ms;
	static u8 time_ms = 0;
	static u8 time_add = 0;
	__disable_irq();
	sys_time_ms = SYS_TIME_MS;
	__enable_irq();	
	
	if(time_ms != sys_time_ms)
	{
		time_ms = sys_time_ms;
		if(Time_Out!=0)
		{
			Time_Out--;
		}
		else
		{
			/*��������������״̬�ǿ���ģʽʱ���ȴ�1.5���ӳ�ʱ*/
			if((WORK_STATE==normal_work_state)&&(NORMAL_WORK_COM_STATE!=rmu_idle_mode))
			{
				if(WORK_COM_PORT==com_by_eth)
				{USART2_Tx_Chars((u8 *)send_pc_fail_ack,3);	}
				else if(WORK_COM_PORT==com_by_com)
				{USART1_Tx_Chars((u8 *)send_pc_fail_ack,3);	}

				LED_BUZZER_CMD(fault);
				WORK_STATE=sys_idle_state;
				NORMAL_WORK_COM_STATE=rmu_idle_mode;
			}
			else if(WORK_STATE==shake_hands_state)
			{
				LED_BUZZER_CMD(fault);
				WORK_STATE=sys_idle_state;
				NORMAL_WORK_COM_STATE=rmu_idle_mode;				
			}
		}
		if(UPDATA_WORK_COM_STATE==sys_wait_shake_hands_end_mode)
		{
			if(++time_add>=200)
			{
				time_add=0;
				FLASH_Unlock();
        FLASH_ErasePage(0x08002000);
        FLASH_Lock();
				NVIC_GenerateSystemReset();
			}
		}
	}
}

/*==================================================================================
* �� �� ���� App_Start
* ��    ���� None
* ��������:  ��������ʱ��İ�������������������ʼ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void App_Start(void)
{
	if(WORK_STATE==sys_idle_state)
	{
		WORK_STATE=normal_work_state;
		if(NORMAL_WORK_COM_STATE==rmu_idle_mode)
		{
			Time_Out=Time_Out_Reload_A;
			USART3_Tx_Chars((u8 *)rlm_wakeup_data,2);//����rlmֹͣ��ǰ�Ĳ���
			NORMAL_WORK_COM_STATE=rmu_wait_rlm_wakeup_mode;
		}
	}
}

/*==================================================================================
* �� �� ���� shake_hands_task
* ��    ���� None
* ��������:  ��ʱ����������Ϣ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void shake_hands_task(void)
{
	u8 sys_time_ms;

	static u8 time_ms = 0;
	__disable_irq();
	sys_time_ms = SYS_TIME_MS;
	__enable_irq();	
	
	if(time_ms != sys_time_ms)
	{
		time_ms = sys_time_ms;
		if(++shake_hands_time_add>=5000)
		{
			if(WORK_STATE==sys_idle_state)
			{
				shake_hands_time_add=0;
				shake_hands_start();
			}
			else
			{shake_hands_time_add=5000;}
		}
	}
}
/*==================================================================================
* �� �� ���� COMMUNICATE_Variable_Init
* ��    ���� None
* ��������:  ��ʼ��ϵͳͨ����Ҫ����
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void COMMUNICATE_Variable_Init(void)
{
	WORK_STATE=sys_idle_state;
	WORK_COM_PORT = com_by_com;		//��ʼ����������ʱ��ͨ�Žӿ�Ϊ��eth
	NORMAL_WORK_COM_STATE=rmu_idle_mode;
	UPDATA_WORK_COM_STATE=sys_idle_mode;
	USART1_Tx(0x33);
}

/*==================================================================================
* �� �� ���� COMMUNICATE_TO_RLM_Run
* ��    ���� None
* ��������:  ����ͨ�Žӿ����ݰ�������Ӧ����
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void COMMUNICATE_TO_RLM_Run(void)
{
	static u8 usart1_rxdend_con = 0;
	static u8 usart2_rxdend_con = 0;
	static u8 usart3_rxdend_con = 0;
	
	time_out_serve();
	shake_hands_task();
	/* ����1�Ƿ���յ������ݰ��ж� */
	if(usart1_rxdend_con!=USART1_RXDEND_CON)
	{
		usart1_rxdend_con=USART1_RXDEND_CON;
		shake_hands_time_add=0;
		switch (WORK_STATE)
		{
			case sys_idle_state :
				deal_usart1_rxd_in_idle();
				break;
			case normal_work_state :
				if(WORK_COM_PORT == com_by_com)
				{
					deal_usart1_rxd_in_normal_work();
				}
				break;
			case passthrough_state :
				deal_usart1_rxd_in_passthrough();
				break;
			case shake_hands_state :
				if(WORK_COM_PORT == com_by_com)
				{
					deal_usart1_rxd_in_shake_hands();
				}
				break;
			default:
				break;
		}
	}
	
	/* ����2�Ƿ���յ������ݰ��ж� */
	if(usart2_rxdend_con!=USART2_RXDEND_CON)
	{
		usart2_rxdend_con=USART2_RXDEND_CON;
		shake_hands_time_add=0;
		switch (WORK_STATE)
		{
			case sys_idle_state :
				deal_usart2_rxd_in_idle();
				break;
			case normal_work_state :
				if(WORK_COM_PORT == com_by_eth)
				{
					deal_usart2_rxd_in_normal_work();
				}
				break;
			case passthrough_state :
				if(WORK_COM_PORT == com_by_eth)
				{
					deal_usart2_rxd_in_passthrough();
				}
				break;
			case shake_hands_state :
				if(WORK_COM_PORT == com_by_eth)
				{
					deal_usart2_rxd_in_shake_hands();
				}
				break;
			default:
				break;
		}
	}
	
	/* ����3�Ƿ���յ������ݰ��ж� */
	if(usart3_rxdend_con!=USART3_RXDEND_CON)
	{
		usart3_rxdend_con=USART3_RXDEND_CON;
		switch (WORK_STATE)
		{
			case normal_work_state:
				deal_usart3_rxd_in_normal_work();
				Time_Out=Time_Out_Reload_A;
				break;
			case passthrough_state :
				if(WORK_COM_PORT == com_by_com)
				{
					USART1_Tx_Chars(USART3_RXD_DATA,USART3_RXD_CON);
				}
				else
				{
					USART2_Tx_Chars(USART3_RXD_DATA,USART3_RXD_CON);
				}
				break;
			default:
				break;
		}
	}
}
