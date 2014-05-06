#define PERIPHERAL_GLOBALS
#include "sys_peripheral.h"

static vu32 TimingDelay=0;
/*==================================================================================
* �� �� ���� wait_sys_peri_ready
* ��    ���� None
* ��������:  ��λ����ʱ���ȴ�����reday
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
static void wait_sys_peri_ready(void)
{
	u32 i;
	for(i=0;i<100000;i++);
}

/*==================================================================================
* �� �� ���� rcc_init
* ��    ���� None
* ��������:  rcc��ʼ��
* �� �� ֵ�� None
* ��    ע�� ��ʼ��ϵͳʱ�ӣ���Ҫע��stm32f10x.h�ж�ϵͳʱ�ӵĶ���
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
static void rcc_init(void)
{
    ErrorStatus HSEStartUpStatus;

		RCC_DeInit();
		
		wait_sys_peri_ready();
		
    /* Enable HSE */  
    RCC_HSEConfig(RCC_HSE_ON);
    //RCC_HSEConfig(RCC_HSE_Bypass);//�ⲿ����Ϊ24M��Դ����
    
    /* Wait till HSE is ready */
    HSEStartUpStatus = RCC_WaitForHSEStartUp(); 
    
    if (HSEStartUpStatus == SUCCESS)
    {    
				FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
				
				FLASH_SetLatency(FLASH_Latency_2);
				
				RCC_HCLKConfig(RCC_SYSCLK_Div1); 
				
				RCC_PCLK1Config(RCC_HCLK_Div2);//����ʱ�����36M
				
				RCC_PCLK2Config(RCC_HCLK_Div1);
				
				RCC_ADCCLKConfig(RCC_PCLK2_Div6);
		
        /* PLLCLK = 24MHz * 3 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);
        
        /* Enable PLL */
        RCC_PLLCmd(ENABLE);
        
        /* Wait till PLL is ready */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {}
        
        /* Select PLL as system clock source */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        /* Wait till PLL is used as system clock source */
        while (RCC_GetSYSCLKSource() != 0x08)
        {}
    }
    RCC_ClockSecuritySystemCmd(ENABLE);  //Enable CSSON��Clock securuty system��  
    
		/* Enable the LSI OSC */
	  RCC_LSICmd(ENABLE);									 //Ϊ�������Ź��ṩʱ��
	  
	  /* Wait till LSI is ready */
	  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	  {}
}

/*==================================================================================
* �� �� ���� peri_clk_init
* ��    ���� None
* ��������:  ΪӦ�õ������ʹ��ʱ��
* �� �� ֵ�� None
* ��    ע�� ��Ҫע���ʼ�������ʱ���е�Deinit������ص��������ʱ�ӣ�Ҫע��˳��
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void peri_clk_init(void)
{	    
      /* Enable WWDG, TIM2, TIM4, USART2 and USART3 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG | RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4 | \
  											 RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3 , ENABLE);    
    
      /* Enable GPIOA, GPIOB, GPIOC, USART1 and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | \
                         RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO , ENABLE);
}

/*==================================================================================
* �� �� ���� NVIC_Configuration
* ��    ���� None
* ��������:  NVIC ��ʼ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
static void NVIC_Configuration(void)
{
		//������IAR��Project->options->C/C++ Compiler->preprocessor->Defined symbols
    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2800);		//release�汾ʱ��IAP���

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//ѡ���ж�Ƕ�׷��飨1��ʱ��1λ��ռ���ȼ���3λ��Ӧ���ӣ����ȼ���
    NVIC_InitTypeDef NVIC_InitStructure;
    
		/* Enable the WWDG Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the USART3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the TIM2 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the TIM4 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*==================================================================================
* �� �� ���� wwdg_init
* ��    ���� None
* ��������:  ���ڿ��Ź���ʼ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
static void wwdg_init(void)
{ 
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(0x50);
    WWDG_Enable(0x7F);
    WWDG_ClearFlag();
    WWDG_EnableIT();
}

/*==================================================================================
* �� �� ���� iwdg_init
* ��    ���� None
* ��������:  �������Ź���ʼ��
* �� �� ֵ�� None
* ��    ע�� ��Ҫ��LSIΪ�������Ź��ṩʱ��
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/ 
static void iwdg_init(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_16);
  IWDG_SetReload(2500-1);
  IWDG_ReloadCounter();
  IWDG_Enable();
}

/*==================================================================================
* �� �� ���� pins_init
* ��    ���� None
* ��������:  ��ʼ������IO״̬
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void pins_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //////////////////////////////////////////////////////
    //
    //                    UART����
    //
    //////////////////////////////////////////////////////
    //uart1 rx,tx pin definition
    // Configure USART1 Rx as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Configure USART1 Tx as alternate function push-pull 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
		//uart2 rx,tx pin definition
    // Configure USART3 Rx as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Configure USART3 TX as alternate function push-pull 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

		
		//uart3 rx,tx pin definition
    // Configure USART3 Rx as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // Configure USART3 TX as alternate function push-pull 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#ifdef NEW_BOARD 
		//////////////////////////////////////////////////////
    //
    //                  �ⲿ��������
    //
    //////////////////////////////////////////////////////
    // Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    

		//////////////////////////////////////////////////////
    //
    //                  Led����
    //
    //////////////////////////////////////////////////////
    // Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_6);
    
#else    
    //////////////////////////////////////////////////////
    //
    //                  �ⲿ��������
    //
    //////////////////////////////////////////////////////
    // Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    

		//////////////////////////////////////////////////////
    //
    //                  Led����
    //
    //////////////////////////////////////////////////////
    // Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_0);
#endif

}

/*==================================================================================
* �� �� ���� uart1_init
* ��    ���� r_baud�������ʴ���
* ��������:  ��ʼ������1
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void uart1_init(u8 r_baud)
{
    u32 BaudRate;
    USART_InitTypeDef USART_1_InitStructure;

    switch (r_baud)
    {
        case 0x00:
            BaudRate = 9600;
            break;
        case 0x01:
            BaudRate = 19200;
            break;
        case 0x02:
            BaudRate = 57600;
            break;
        case 0x03:
            BaudRate = 115200;
            break;   
        default:
            BaudRate = 57600;
            break;
    }
    
    // UART1 config --------------------------------------------------
    USART_1_InitStructure.USART_BaudRate = BaudRate;    
    USART_1_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_1_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_1_InitStructure.USART_Parity = USART_Parity_No;
    USART_1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);      // receive interrupt enable
    USART_ITConfig(USART1, USART_IT_TXE,  DISABLE);      // transmit interrupt disable

    USART_Init(USART1, &USART_1_InitStructure);    
    USART_Cmd(USART1, ENABLE);                          	// enable UART1        
}

/*==================================================================================
* �� �� ���� uart2_init
* ��    ���� r_baud�������ʴ���
* ��������:  ��ʼ������2
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void uart2_init(u8 r_baud)
{
    u32 BaudRate;
    USART_InitTypeDef USART_2_InitStructure;

    switch (r_baud)
    {
        case 0x00:
            BaudRate = 9600;
            break;
        case 0x01:
            BaudRate = 19200;
            break;
        case 0x02:
            BaudRate = 57600;
            break;
        case 0x03:
            BaudRate = 115200;
            break;   
        default:
            BaudRate = 57600;
            break;
    }
    
    // UART1 config --------------------------------------------------
    USART_2_InitStructure.USART_BaudRate = BaudRate;    
    USART_2_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_2_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_2_InitStructure.USART_Parity = USART_Parity_No;
    USART_2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);      // receive interrupt enable
    USART_ITConfig(USART2, USART_IT_TXE,  DISABLE);      // transmit interrupt disable
    
    USART_Init(USART2, &USART_2_InitStructure);    
    USART_Cmd(USART2, ENABLE);                          	// enable UART1        
}

/*==================================================================================
* �� �� ���� uart3_init
* ��    ���� r_baud�������ʴ���
* ��������:  ��ʼ������3
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void uart3_init(u8 r_baud)
{
    u32 BaudRate;
    USART_InitTypeDef USART_3_InitStructure;

    switch (r_baud)
    {
        case BAUD_RATE_9600:
            BaudRate = 9600;
            break;
        case BAUD_RATE_19200:
            BaudRate = 19200;
            break;
        case BAUD_RATE_57600:
            BaudRate = 57600;
            break;
        case BAUD_RATE_115200:
            BaudRate = 115200;
            break;   
        default:
            BaudRate = 57600;
            break;
    }
    
    // UART1 config --------------------------------------------------
    USART_3_InitStructure.USART_BaudRate = BaudRate;    
    USART_3_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_3_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_3_InitStructure.USART_Parity = USART_Parity_No;
    USART_3_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_3_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);      // receive interrupt enable
    USART_ITConfig(USART3, USART_IT_TXE,  DISABLE);      // transmit interrupt disable

    USART_Init(USART3, &USART_3_InitStructure);    
    USART_Cmd(USART3, ENABLE);                          	// enable UART1        
}

/*==================================================================================
* �� �� ���� timer2_init
* ��    ���� None
* ��������:  ��ʼ��TIM2
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void timer2_init(void)
{   
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
      
    TIM_TimeBaseStructure.TIM_Period = 20000; //0.910222222ms * (1098 + 1)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    //Timer2 clock source configured as Intenal Clock
    TIM_InternalClockConfig(TIM2);
        
    //Set Timer2 prescaler to 65535,��ƵΪ1.0986328125KHz(0.910222222ms)
    TIM_PrescalerConfig(TIM2, 65535, TIM_PSCReloadMode_Immediate );    
 
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    TIM_SetCounter(TIM2,0);
 
 		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
 		
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    TIM_Cmd(TIM2, ENABLE);
}

/*==================================================================================
* �� �� ���� timer4_init
* ��    ���� None
* ��������:  ��ʼ��TIM4
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void timer4_init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   
    TIM_DeInit(TIM4); 
    
    TIM_TimeBaseStructure.TIM_Period = 49; //0.910222222ms * (49 + 1)
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    //Timer4 clock source configured as Intenal Clock
    TIM_InternalClockConfig(TIM4);
    
    //Set Timer4 prescaler to 65535,��ƵΪ1.0986328125KHz(0.910222222ms)
    TIM_PrescalerConfig(TIM4, 65535, TIM_PSCReloadMode_Immediate );
    
    TIM_ARRPreloadConfig(TIM4, ENABLE);
    
    TIM_SetCounter(TIM4,0);
   
   	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
   	
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    
    TIM_Cmd(TIM4, ENABLE); 
}

/*==================================================================================
* �� �� ���� systick_init
* ��    ���� None
* ��������:  ��ʼ��ϵͳ��શ�ʱ��
* �� �� ֵ�� None
* ��    ע�� �˺������õ�SysTick_CLKSourceConfig()�趨��ʱ��������ʱ�ӣ����ǲ�֪��Ϊ
						 ʲô����SysTick_CLKSource_HCLK_Div8��SysTick_CLKSource_HCLK�����������
						 ����ʱ����һ�����д���֤��
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void systick_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	if (SysTick_Config(72000))
  { 
    while (1);
  }
}

/*==================================================================================
* �� �� ���� sys_variable_init
* ��    ���� None
* ��������:  ϵͳӦ�ó�������ĳ�ʼ������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void sys_variable_init(void)
{
	__disable_irq();
	SYS_TIME_MS=0;
	__enable_irq();	
	USART1_Variable_Init();
	USART2_Variable_Init();
	USART3_Variable_Init();	
	COMMUNICATE_Variable_Init();
} 

/*==================================================================================
* �� �� ���� Delay_Ms
* ��    ���� nTime������ʱʱ��=nTime ms
* ��������:  ��ʱ����ʱ�������뼶
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void Delay_Ms(u32 nTime)
{ 
  TimingDelay = nTime;
  while(TimingDelay != 0)
  {}
} 

/*==================================================================================
* �� �� ���� Timingdelay_Decrement
* ��    ���� None
* ��������:  Delay_Ms()�������뼶��ʱ���������º���
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void Timingdelay_Decrement(void)
{
  if (TimingDelay != 0)
  { 
    TimingDelay--;
  }
}

/*==================================================================================
* �� �� ���� Reset_Wwdg
* ��    ���� None
* ��������:  ��λ���ڿ��Ź���ʱ��
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void Reset_Wwdg(void)
{    
	WWDG_SetCounter(WWDG_COUNTER);
}

/*==================================================================================
* �� �� ���� NVIC_GenerateSystemReset
* ��    ���� None
* ��������:  ������λ����
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void NVIC_GenerateSystemReset(void)
{
    SCB->AIRCR = AIRCR_VECTKEY_MASK | (u32)0x04;
}
/*==================================================================================
* �� �� ���� WWDG_Reload_Task
* ��    ���� None
* ��������:  ���ڿ��Ź���������
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void WWDG_Reload_Task(void)
{
	u8 sys_time_ms;
	static u8 time_ms = 0;
	static u8 time_add_up;
	__disable_irq();
	sys_time_ms = SYS_TIME_MS;
	__enable_irq();	
	
	if(time_ms != sys_time_ms)
	{
		time_ms = sys_time_ms;
		if(++time_add_up>=50)
		{
			time_add_up=0;
			Reset_Wwdg();
		}
	}
}

/*==================================================================================
* �� �� ���� Set_System
* ��    ���� None
* ��������:  ��ʼ��ϵͳ
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
void Set_System(void)
{  
    rcc_init();

    peri_clk_init();  
      
  	NVIC_Configuration();  
  	
    pins_init();  
    
    systick_init();
    
    //timer2_init();
    //timer4_init(); 
    
    uart1_init(BAUD_RATE_57600);	
		uart2_init(BAUD_RATE_57600);
		uart3_init(BAUD_RATE_57600);

		iwdg_init();
	  wwdg_init();
		  
    sys_variable_init();
}
