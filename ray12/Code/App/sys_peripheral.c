#define PERIPHERAL_GLOBALS
#include "sys_peripheral.h"

#define DAC_DHR8R1_Address      0x40007410

const uint8_t Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

vu32 TimingDelay=0;
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
    RCC_HSEConfig(RCC_HSE_Bypass);//�ⲿ����Ϊ24M��Դ����
    
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
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM6 | RCC_APB1Periph_SPI2 | \
  											 RCC_APB1Periph_USART2 | RCC_APB1Periph_DAC | RCC_APB1Periph_PWR, ENABLE);    
    
      /* Enable GPIOA, GPIOB, GPIOC, USART1 and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_GPIOB  | RCC_APB2Periph_GPIOC  | RCC_APB2Periph_ADC1 | \
                         RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO , ENABLE);
                         
      /* Enable DMA2 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
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
    
    
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    
    /* Enable the TIM2 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* Enable the TIM4 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

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
    // Configure USART2 Rx as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // Configure USART2 TX as alternate function push-pull 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //////////////////////////////////////////////////////
    //
    //                  �ⲿ��������
    //
    //////////////////////////////////////////////////////
    // Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);     
		// Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
		// Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
    // Configure key pin as input floating 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    //////////////////////////////////////////////////////
    //
    //           ��Ƶ����
    //
    //////////////////////////////////////////////////////
    //DAC����ܽţ�һ��DACͨ��������,��Ӧ��GPIO�����Զ����ӵ�DACת������Ϊ�˱����������,���GPIO����Ӧ��������ģ������
   	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		//NCP2820ʹ������
   	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
}
static void  LCD_Config (void)
{
    SPI_InitTypeDef   SPI_init;
    GPIO_InitTypeDef  gpio_init;
    
    gpio_init.GPIO_Pin   = LCD_PWR_Pin;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_PWR_Port, &gpio_init);    
    
    gpio_init.GPIO_Pin   = LCD_RST_Pin | LCD_DC_Pin | LCD_CS_Pin;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);
    
    // for SPI interface
    gpio_init.GPIO_Pin   = LCD_SCK_Pin | LCD_MOSI_Pin;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gpio_init);
    
    // enable OLED PWREN vcc = 16v
    GPIO_ResetBits(LCD_PWR_Port, LCD_PWR_Pin);

		GPIO_ResetBits(LCD_CS_Port, LCD_CS_Pin);   
    GPIO_ResetBits(LCD_RST_Port, LCD_RST_Pin);
    Delay_Ms(10);
    GPIO_SetBits(LCD_RST_Port, LCD_RST_Pin); 
    GPIO_SetBits(LCD_CS_Port, LCD_CS_Pin);

    SPI_init.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_init.SPI_Mode = SPI_Mode_Master;
    SPI_init.SPI_DataSize = SPI_DataSize_8b;
    SPI_init.SPI_CPOL = SPI_CPOL_High;
    SPI_init.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_init.SPI_NSS = SPI_NSS_Soft;
    SPI_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_init.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_init.SPI_CRCPolynomial = 7;


    SPI_I2S_DeInit(SPI2); 
    SPI_Cmd(SPI2, DISABLE); 
    SPI_Init(SPI2, &SPI_init); 
    SPI_Cmd(SPI2, ENABLE);
    SPI_CalculateCRC(SPI2, DISABLE);
}

static void RAY12_ADC_Init (void)
{
    // for ADC
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Configure PC.00 (ADC Channel10) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	ADC_InitTypeDef  ADC_InitStructure;
  /* ADC1 Configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                    // ADC1 �ڶ���ģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                         // ENABLE-ADC��ͨ��ɨ��, DISABLE-ADC��ͨ��ɨ��
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                    // ENABLE--ADC����ת��ģʽ DISABLE--ADC����ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   // ����������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                // �������Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;                               // ����ת��1��ADͨ��ֵ
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel10 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_13Cycles5);

  /* Enable AWD interupt */
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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

void clear_guanji_counter(void)
{
	guanji_times_add_up=0;
	TIM_SetCounter(TIM2,0);
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
    
    TIM_TimeBaseStructure.TIM_Period = 65535; //0.910222222ms * (32958 + 1) = 60s
    
    TIM_TimeBaseStructure.TIM_Prescaler = 65535;
    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    //Timer2 clock source configured as Intenal Clock
    TIM_InternalClockConfig(TIM2); 
 		
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
    TIM_SetCounter(TIM2,0);    
 		
 		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    
    TIM_Cmd(TIM2, ENABLE);
}

/*==================================================================================
* �� �� ���� dac1_init
* ��    ���� None
* ��������:  ��ʼ��dac1
* �� �� ֵ�� None
* ��    ע�� 
* ��    �ߣ� gaodb
* ����ʱ�䣺 2012.10
==================================================================================*/
static void dac1_init(void)
{
	DAC_InitTypeDef            DAC_InitStructure;
	DMA_InitTypeDef            DMA_InitStructure;
  TIM_TimeBaseInitTypeDef  	 TIM_TimeBaseStructure;
  
  TIM_TimeBaseStructure.TIM_Period = 0xf; //0.910222222ms * (1089 + 1) = 1s
  TIM_TimeBaseStructure.TIM_Prescaler = 0xff;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
  
  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	TIM_ARRPreloadConfig(TIM6, ENABLE);
	
  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  
  DMA_DeInit(DMA2_Channel3);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Escalator8bit;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
  /* Enable DMA2 Channel3 */
  DMA_Cmd(DMA2_Channel3, ENABLE);
  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
     automatically connected to the DAC converter. */
	DAC_Cmd(DAC_Channel_1, ENABLE);
  /* Enable DMA for DAC Channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);
  /* TIM6 enable counter */
  TIM_Cmd(TIM6, ENABLE);
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
    
    TIM_TimeBaseStructure.TIM_Period = 1089; //0.910222222ms * (1089 + 1)
    TIM_TimeBaseStructure.TIM_Prescaler = 65535;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    //Timer4 clock source configured as Intenal Clock
    TIM_InternalClockConfig(TIM4);
       
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
	KEY_Variable_Init();
	COMMUNICATE_Variable_Init();
	clear_guanji_counter();
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
		
    timer2_init();
    timer4_init(); 
    
    RAY12_ADC_Init();
    
    LCD_Config();    
    DispInit();
    
    uart1_init(BAUD_RATE_57600);	
		uart2_init(BAUD_RATE_57600);
		
		dac1_init();
		
    sys_variable_init();
}
