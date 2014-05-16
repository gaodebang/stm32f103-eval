#define		COMMUNICATE_GLOBALS
#include "communicate_to_rlm.h"

#define Time_Out_Reload (1000)



static u32 Time_Out=0;

static u8 Time_temp[10];

static u8 TID[30]={1,2,3,4,5,6};

static uc8 rlm_wakeup_data[]						=	{0x02,0x12};
static uc8 rlm_get_user_data[]					=	{0x09,0x20,0x00,0x00,0x00,0x00,0x03,0,1};
static uc8 rlm_get_tid_data[]						=	{0x09,0x20,0x00,0x00,0x00,0x00,0x03,10,1};

/*==================================================================================
* 函 数 名： crccalc
* 参    数： u8* pTagFrame,u16 len,u8* pTagCrc
* 功能描述:  CRC校验值计算返回函数
* 返 回 值： None
* 备    注： CRC校验值的返回通过指针
* 作    者： gaodb
* 创建时间： 2012.10
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

static u8 hex_2_asc(u8 datain,u8 high_or_low)
{
	u8 temp_data;
	u8 temp_list[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	if(high_or_low==0)
	{
		temp_data=datain>>4;
	}
	else
	{
		temp_data=datain&0x0f;
	}
	return temp_list[temp_data];
}

void Aes_Deal(u8 *data_temp_sn)
{
	u8 i;
	u16 tim4_counter;
	u8 data_qr_sn[24];

	for(i=0;i<12;i++)
	{
		data_qr_sn[i]=hex_2_asc(data_temp_sn[(i+2)/2],i%2);
	}
	
	EncodeData(data_qr_sn);
	Display_Sn_Qr();
}

static void clr_mark_rtc_data(void)
{
	u8 i;
	for(i=0;i<7;i++)
	{
		Time_temp[i]=*(USART1_RXD_DATA+2+i);
	}
}

static void add_mark_rtc_data(void)
{
  u8 i;
	for(i=0;i<7;i++)
	{
		*(Time_temp+i)=*(Time+i);
	}
}
/*==================================================================================
* 函 数 名： deal_usart1_rxd_in_idle
* 参    数： None
* 功能描述:  系统空闲时串口1接收命令服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
			if(((cmd_temp&0x7f)==0x0B)&&(data_len==0x02))//切换到com与rlm透传模式
			{
				WORK_STATE=passthrough_state;
				Display_Serianet_Mark();
				TIM_Cmd(TIM2, DISABLE);
			}
			else if(((cmd_temp&0x7f)==0x0F)&&(data_len==0x02))//启动一次normal_work 
			{
				Display_wait(0);
				WORK_STATE=normal_work_state;
				App_Start();
			}
			else if(((cmd_temp&0x7f)==0x1F)&&(data_len==0x02))//启动一次on_line_work
			{
				Display_wait(1);
				WORK_STATE=on_line_work_state;
				App_Start();
			}			
			else if(((cmd_temp&0x7f)==0x71)&&(data_len==0x06))
			{
				if((*(USART1_RXD_DATA+2)==0xA0)&&(*(USART1_RXD_DATA+3)==0xA0)&&(*(USART1_RXD_DATA+4)==0x50)&&(*(USART1_RXD_DATA+5)==0x50))
				{
					WORK_STATE=sys_updata_state;
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
* 函 数 名： deal_usart1_rxd_in_passthrough
* 参    数： None
* 功能描述:  系统空闲时串口1接收命令服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
	
	if((cmd_temp==0x0b)||(cmd_temp==0x1b)||(cmd_temp==0x0f)||(cmd_temp==0x1f))
	{
		if(((cmd_temp&0x7f)==0x1B)&&(data_len==0x02))
		{
			WORK_STATE=sys_idle_state;
			USART1_Tx_Chars(set_rmu_cmd_ack,3);
			guanji_times_add_up=0;
			TIM_Cmd(TIM2, ENABLE);
			Display_Welcome();
		}
		else if(((cmd_temp&0x7f)==0x0B)&&(data_len==0x02))
		{
			USART1_Tx_Chars(set_rmu_cmd_ack,3);
		}
	}
	else
	{
		USART2_Tx_Chars(USART1_RXD_DATA,USART1_RXD_CON);
	}		
	return 0;
}
/*==================================================================================
* 函 数 名： deal_usart2_rxd_in_normal_work
* 参    数： None
* 功能描述:  系统主机模式时串口2接收命令服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
		case rmu_wait_rlm_wakeup_mode:
			if(cmd_temp==0x12)
			{
				if(cmd_stat!=0)
				{
					return 0;//命令执行失败，报错，重新开始
				}
				USART2_Tx_Chars((u8 *)rlm_get_user_data,9);
				NORMAL_WORK_COM_STATE=rmu_wait_rlm_user_data_mode;
			}
			break;
		case rmu_wait_rlm_user_data_mode:
			if(cmd_temp==0x20)
			{
				if(cmd_stat!=0)
				{
					return 0;//命令执行失败，报错，重新开始
				}
				Display_Wine_Message(data_satrt_addr);
				GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
				Delay_Ms(300);
				GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
				WORK_STATE=sys_idle_state;
				NORMAL_WORK_COM_STATE=rmu_idle_mode;				
			}
			break;				
		default:
			break;
	}
	return 0;
}

/*==================================================================================
* 函 数 名： deal_usart2_rxd_in_on_line_work
* 参    数： None
* 功能描述:  系统主机模式时串口2接收命令服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
static u8 deal_usart2_rxd_in_on_line_work(void)
{
	u16 data_len;
	u8 cmd_temp;	
	u8 cmd_stat;
	u8 recrc[2];
	u8 * data_satrt_addr;
	//////////////////////
	static u8 jiujingdu[7];
	//////////////////////
	
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
		case rmu_wait_rlm_wakeup_mode:
			if(cmd_temp==0x12)
			{
				if(cmd_stat!=0)
				{
					return 0;//命令执行失败，报错，重新开始			
				}
				USART2_Tx_Chars((u8 *)rlm_get_tid_data,9);
				NORMAL_WORK_COM_STATE=rmu_wait_rlm_tid_data_mode;
			}
			break;
		case rmu_wait_rlm_tid_data_mode:
			if(cmd_temp==0x20)
			{
				if(cmd_stat!=0)
				{
					return 0;//命令执行失败，报错，重新开始
				}
				jiujingdu[0]=*data_satrt_addr;
				jiujingdu[1]=TIM_GetCounter(TIM4)&0xff;
				jiujingdu[2]=TIM_GetCounter(TIM2)&0xff;
				jiujingdu[3]=(TIM_GetCounter(TIM4)&0xff)+0x5a;
				jiujingdu[4]=(TIM_GetCounter(TIM2)&0xff)+0xa5;
				jiujingdu[5]=jiujingdu[1]^jiujingdu[2];
				jiujingdu[6]=jiujingdu[3]^jiujingdu[4];
				jiujingdu_addr=jiujingdu;
				Display_Sn_Data(jiujingdu_addr);
				//Aes_Deal(data_satrt_addr);
				GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
				Delay_Ms(300);
				GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
				WORK_STATE=sys_idle_state;
				NORMAL_WORK_COM_STATE=rmu_dis_sn_mode;
			}
			break;				
		default:
			break;
	}
	return 0;
}

/*==================================================================================
* 函 数 名： time_out_serve
* 参    数： None
* 功能描述:  通信超时服务函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
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
			if((WORK_STATE==on_line_work_state)||(WORK_STATE==normal_work_state))
			{
					App_Start();
			}
		}	
		if(WORK_STATE==sys_updata_state)
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
* 函 数 名： App_Start
* 参    数： None
* 功能描述:  正常工作时候的按键触发的正常工作开始
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void App_Start(void)
{
		clear_guanji_counter();
		Time_Out=Time_Out_Reload;
		USART2_Tx_Chars((u8 *)rlm_wakeup_data,2);//唤醒rlm停止当前的操作
		NORMAL_WORK_COM_STATE=rmu_wait_rlm_wakeup_mode;
}

/*==================================================================================
* 函 数 名： COMMUNICATE_Variable_Init
* 参    数： None
* 功能描述:  初始化系统通信需要参数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void COMMUNICATE_Variable_Init(void)
{
	WORK_STATE=sys_idle_state;
	NORMAL_WORK_COM_STATE=rmu_idle_mode;
	USART1_Tx(0x33);
}

/*==================================================================================
* 函 数 名： COMMUNICATE_TO_RLM_Run
* 参    数： None
* 功能描述:  各个通信接口数据包接收响应函数
* 返 回 值： None
* 备    注： 
* 作    者： gaodb
* 创建时间： 2012.10
==================================================================================*/
void COMMUNICATE_TO_RLM_Run(void)
{
	static u8 usart1_rxdend_con = 0;
	static u8 usart2_rxdend_con = 0;

	time_out_serve();
	/* 串口1是否接收到新数据包判断 */
	if(usart1_rxdend_con!=USART1_RXDEND_CON)
	{
		usart1_rxdend_con=USART1_RXDEND_CON;
		clear_guanji_counter();
		if(WORK_STATE==sys_idle_state)
		{
			deal_usart1_rxd_in_idle();
		}
		else if(WORK_STATE == passthrough_state)
		{
			deal_usart1_rxd_in_passthrough();
		}
	}
	
	/* 串口2是否接收到新数据包判断 */
	if(usart2_rxdend_con!=USART2_RXDEND_CON)
	{
		usart2_rxdend_con=USART2_RXDEND_CON;
		if(WORK_STATE==normal_work_state)
		{
			Time_Out=Time_Out_Reload;
			deal_usart2_rxd_in_normal_work();
		}
		else if(WORK_STATE==on_line_work_state)
		{
			Time_Out=Time_Out_Reload;
			deal_usart2_rxd_in_on_line_work();			
		}
		else if(WORK_STATE==passthrough_state)
		{
			USART1_Tx_Chars(USART2_RXD_DATA,USART2_RXD_CON);
		}
	}	
}