/*
*********************************************************************************************************
*
*	模块名称 : 录音演示程序。
*	文件名称 : audio_rec.c
*	版    本 : V1.0
*	说    明 : 演示I2S录音和回放功能。通过串口超级终端作为操作界面。
*
*********************************************************************************************************
*/

/*
	开发板配置的I2S音频CODEC芯片为WM8978。

	操作提示：
		[TAMPER]键     = 选择音频格式
		[WAKEUP]键     = 开始录音
		[USER]键       = 开始回放
		摇杆上/下键    = 调节音量
		摇杆左/右键    = 调节MIC增益
		摇杆OK键       = 终止录放

	录音时数据保存在安富莱STM32开发板外扩的2MB SRAM中，缺省使用8K采样率，16bit格式。1MB可以保存64秒录音。
	放音按相同的格式，从外部SRAM中读取数据。

	I2S_StartRecord() 和 I2S_StartPlay()函数的形参值域
    接口标准支持：
		#define I2S_Standard_Phillips           ((uint16_t)0x0000)
		#define I2S_Standard_MSB                ((uint16_t)0x0010)
		#define I2S_Standard_LSB                ((uint16_t)0x0020)
		#define I2S_Standard_PCMShort           ((uint16_t)0x0030)
		#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)

    字长支持：
    	#define I2S_DataFormat_16b              ((uint16_t)0x0000)
		#define I2S_DataFormat_16bextended      ((uint16_t)0x0001)
		#define I2S_DataFormat_24b              ((uint16_t)0x0003)
		#define I2S_DataFormat_32b              ((uint16_t)0x0005)

	采样频率支持：
		#define I2S_AudioFreq_192k               ((uint32_t)192000)
		#define I2S_AudioFreq_96k                ((uint32_t)96000)
		#define I2S_AudioFreq_48k                ((uint32_t)48000)
		#define I2S_AudioFreq_44k                ((uint32_t)44100)
		#define I2S_AudioFreq_32k                ((uint32_t)32000)
		#define I2S_AudioFreq_22k                ((uint32_t)22050)
		#define I2S_AudioFreq_16k                ((uint32_t)16000)
		#define I2S_AudioFreq_11k                ((uint32_t)11025)
		#define I2S_AudioFreq_8k                 ((uint32_t)8000)
*/

#include "app.h"
#include <inttypes.h>

REC_T g_tRec;

/* 音频格式切换列表(可以自定义) */
#define FMT_COUNT	6		/* 音频格式数组元素个数 */
uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};


/* 仅允许本文件内调用的函数声明 */
static void StartRecord(void);
static void StopRec(void);

/*
*********************************************************************************************************
*	函 数 名: RecorderDemo
*	功能说明: 录音机主程序
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void RecorderDemo(void)
{
	/* 配置外部SRAM接口已经在 bsp.c 中的 bsp_Init() 函数执行过 */

	/* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
	if (!wm8978_Init())
	{
		bsp_DelayMS(2000);
		return;
	}

	/* 初始化全局变量 */
	g_tRec.ucVolume = 39;		/* 缺省音量 */
	g_tRec.ucMicGain = 34;			/* 缺省PGA增益 */

	I2S_CODEC_Init();			/* 初始化设置I2S */

	g_tRec.ucFmtIdx = 1;		/* 缺省音频格式(16Bit, 16KHz) */
	g_tRec.pAudio = aAudio;	/* 通过 init16_t * 型指针访问外部SRAM */

	/* 清零录音缓冲区. SRAM容量2M字节 */
	{
		int i;

		for (i = 0 ; i < FFT_LEN; i++)
		{
			g_tRec.pAudio[i] = 0;
		}
	}

	StartRecord();
}

/*
*********************************************************************************************************
*	函 数 名: StartRecord
*	功能说明: 配置WM8978和STM32的I2S开始录音。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void StartRecord(void)
{
	/* 如果已经再录音和放音状态，则需要先停止再开启 */
	g_tRec.ucStatus = STA_STOP_I2S;
	bsp_DelayMS(10);	/* 延迟一段时间，等待I2S中断结束 */
	wm8978_Init();		/* 复位WM8978到复位状态 */
	I2S_Stop();			/* 停止I2S录音和放音 */

	g_tRec.ucStatus = STA_RECORDING;		/* 录音状态 */

	g_tRec.uiCursor = 0;	/* 数据指针复位为0，从头开始录音 */

	/* 配置WM8978芯片，输入为Mic，输出为耳机 */
	//wm8978_CfgAudioPath(MIC_LEFT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	//wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* 调节放音音量，左右相同音量 */
	wm8978_SetEarVolume(g_tRec.ucVolume);

	/* 设置MIC通道增益 */
	wm8978_SetMicGain(g_tRec.ucMicGain);

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	/* 配置STM32的I2S音频接口(比如飞利浦标准I2S接口，16bit， 8K采样率), 开始录音 */
	I2S_StartRecord(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
}

/*
*********************************************************************************************************
*	函 数 名: StopRec
*	功能说明: 停止录音和放音
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void StopRec(void)
{
	g_tRec.ucStatus = STA_IDLE;		/* 待机状态 */
	I2S_Stop();		/* 停止I2S录音和放音 */
	wm8978_Init();	/* 复位WM8978到复位状态 */
}

/*
*********************************************************************************************************
*	函 数 名: I2S_CODEC_DataTransfer
*	功能说明: I2S数据传输函数, 被SPI2 (I2S)中断服务程序调用。录音和放音均在此函数处理。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void I2S_CODEC_DataTransfer(void)
{
	uint16_t usData;

	/* 只录左声道(安富莱STM32开发板MIC安装在左声道), 放音时左右声道相同 */
	if (g_tRec.ucStatus == STA_RECORDING)	/* 录音状态 */
	{
		if (SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE) == SET)
		{
			usData = SPI_I2S_ReceiveData(I2S2ext);
			g_tRec.pAudio[g_tRec.uiCursor] = usData;	/* 保存音频数据 */
			if (++g_tRec.uiCursor >= FFT_LEN)
			{
			    if(Fft_Start_Mark == 0)
			    {
				    g_tRec.uiCursor = 0;
				    Fft_Start_Mark = 1;
				   	Load_Data_FFT(g_tRec.pAudio,FFT_LEN);
                }
                else
                {
                    g_tRec.uiCursor = FFT_LEN;
                }
			}
		}

		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
		{
			SPI_I2S_SendData(SPI2, 0);
		}
	}
	else if (g_tRec.ucStatus == STA_PLAYING)	/* 放音状态，右声道的值等于左声道 */
	{
		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
		{
			#if 1		/* 双通道放音 */
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* 读取音频数据 */
				if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
				{
					g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
					/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
					SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
					/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
					SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

					bsp_PutKey(JOY_DOWN_OK);	/* 模拟停止键按下 */
				}
				SPI_I2S_SendData(SPI2, usData);
			#else	/* 可以将其中1个通道的声音复制到另外1个通道(用于左声道录音或右声道录音) */
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* 读取音频数据 */
				if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE) != SET)	/* 判断是不是左声道 */
				{
					if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
					{
						g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
						/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
						SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
						/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
						SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

						bsp_PutKey(JOY_DOWN_OK);	/* 模拟停止键按下 */
					}
				}
				SPI_I2S_SendData(SPI2, usData);
			#endif
		}
	}
	else 	/* 临时状态 STA_STOP_I2S，用于停止中断 */
	{
		/* 禁止I2S2 RXNE中断(接收不空)，需要时再打开 */
		SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
		/* 禁止I2S2 TXE中断(发送缓冲区空)，需要时再打开 */
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
	}
}

/*
*********************************************************************************************************
*	函 数 名: SPI2_IRQHandler
*	功能说明: I2S数据传输中断服务程序。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void SPI2_IRQHandler(void)
{
	I2S_CODEC_DataTransfer();
}

