/*
*********************************************************************************************************
*
*	模块名称 : RTC模块(For STM32F4XX)
*	文件名称 : bsp_rtc.c
*	版    本 : V1.0
*	说    明 : 
*
*********************************************************************************************************
*/

#include "bsp.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef RTC_InitStructure;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;
RTC_TimeTypeDef  RTC_TimeStampStructure;
RTC_DateTypeDef  RTC_TimeStampDateStructure;
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;



/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  SynchPrediv = 0xFF;
  AsynchPrediv = 0x7F;

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Enable The TimeStamp */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, DISABLE);    
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeRegulate(void)
{
    uint8_t error1, error2;
    printf("\r\n==============Time Settings=====================================\r\n");
    RTC_TimeStructure.RTC_H12       = RTC_H12_PM;   //RTC_H12_AM or RTC_H12_PM
    RTC_TimeStructure.RTC_Hours     = 22;
    RTC_TimeStructure.RTC_Minutes   = 30;
    RTC_TimeStructure.RTC_Seconds   = 0;
    error1 = RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_DateStructure.RTC_WeekDay   = 7;
    RTC_DateStructure.RTC_Date      = 1;
    RTC_DateStructure.RTC_Month     = 9;
    RTC_DateStructure.RTC_Year      = 13;
    error2 = RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
    if((error1 != ERROR)&&(error2 != ERROR))
    {
        RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
    }
    else
    {
        printf("\r\n==============Settings Time failed.==========================\r\n");        
    }
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time and Date */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  printf("\r\n\r\n============== Current Time Display ============================\n");
  printf("\r\n  The current time (Hour-Minute-Second) is :  %0.2d:%0.2d:%0.2d \n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
  /* Unfreeze the RTC DR Register */
  (void)RTC->DR;
}

/**
  * @brief  Display the current date on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_DateShow(void)
{
  /* Get the current Date */
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
  printf("\r\n============== Current Date Display ============================\n");
  printf("\r\n  The current date (WeekDay-Date-Month-Year) is :  %0.2d-%0.2d-%0.2d-%0.2d \n", RTC_DateStructure.RTC_WeekDay, RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year);
}

/*
*********************************************************************************************************
*	函 数 名: Bsp_Rtc_Init
*	功能说明: 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Rtc_Init(void)
{
    //if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
    if(1)
    {
        printf("\r\n Need to configure RTC....\r\n");
        
        /* RTC configuration  */
        RTC_Config();
        
        /* Configure the RTC data register and RTC prescaler */
        RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
        RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
        RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
        
        /* Check on RTC init */
        if (RTC_Init(&RTC_InitStructure) == ERROR)
        {
            printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \r\n");
        }
        
        /* Configure the time register */
        RTC_TimeRegulate(); 
    }
    else
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            //printf("\r\n Power On Reset occurred....\r\n");
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            //printf("\r\n External Reset occurred....\r\n");
        }

        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        
        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);
        
        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
        
        /* Clear the RTC Alarm Flag */
        RTC_ClearFlag(RTC_FLAG_ALRAF); 
    }
}

