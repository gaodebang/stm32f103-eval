/*
*********************************************************************************************************
*
*	ģ������ : ¼����ʾ����
*	�ļ����� : audio_rec.c
*	��    �� : V1.0
*	˵    �� : ��ʾI2S¼���ͻطŹ��ܡ�ͨ�����ڳ����ն���Ϊ�������档
*
*********************************************************************************************************
*/

/*
	���������õ�I2S��ƵCODECоƬΪWM8978��

	������ʾ��
		[TAMPER]��     = ѡ����Ƶ��ʽ
		[WAKEUP]��     = ��ʼ¼��
		[USER]��       = ��ʼ�ط�
		ҡ����/�¼�    = ��������
		ҡ����/�Ҽ�    = ����MIC����
		ҡ��OK��       = ��ֹ¼��

	¼��ʱ���ݱ����ڰ�����STM32������������2MB SRAM�У�ȱʡʹ��8K�����ʣ�16bit��ʽ��1MB���Ա���64��¼����
	��������ͬ�ĸ�ʽ�����ⲿSRAM�ж�ȡ���ݡ�

	I2S_StartRecord() �� I2S_StartPlay()�������β�ֵ��
    �ӿڱ�׼֧�֣�
		#define I2S_Standard_Phillips           ((uint16_t)0x0000)
		#define I2S_Standard_MSB                ((uint16_t)0x0010)
		#define I2S_Standard_LSB                ((uint16_t)0x0020)
		#define I2S_Standard_PCMShort           ((uint16_t)0x0030)
		#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)

    �ֳ�֧�֣�
    	#define I2S_DataFormat_16b              ((uint16_t)0x0000)
		#define I2S_DataFormat_16bextended      ((uint16_t)0x0001)
		#define I2S_DataFormat_24b              ((uint16_t)0x0003)
		#define I2S_DataFormat_32b              ((uint16_t)0x0005)

	����Ƶ��֧�֣�
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

/* ��Ƶ��ʽ�л��б�(�����Զ���) */
#define FMT_COUNT	6		/* ��Ƶ��ʽ����Ԫ�ظ��� */
uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};


/* ���������ļ��ڵ��õĺ������� */
static void StartRecord(void);
static void StopRec(void);

/*
*********************************************************************************************************
*	�� �� ��: RecorderDemo
*	����˵��: ¼����������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RecorderDemo(void)
{
	/* �����ⲿSRAM�ӿ��Ѿ��� bsp.c �е� bsp_Init() ����ִ�й� */

	/* ���WM8978оƬ���˺������Զ�����CPU��GPIO */
	if (!wm8978_Init())
	{
		bsp_DelayMS(2000);
		return;
	}

	/* ��ʼ��ȫ�ֱ��� */
	g_tRec.ucVolume = 39;		/* ȱʡ���� */
	g_tRec.ucMicGain = 34;			/* ȱʡPGA���� */

	I2S_CODEC_Init();			/* ��ʼ������I2S */

	g_tRec.ucFmtIdx = 1;		/* ȱʡ��Ƶ��ʽ(16Bit, 16KHz) */
	g_tRec.pAudio = aAudio;	/* ͨ�� init16_t * ��ָ������ⲿSRAM */

	/* ����¼��������. SRAM����2M�ֽ� */
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
*	�� �� ��: StartRecord
*	����˵��: ����WM8978��STM32��I2S��ʼ¼����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void StartRecord(void)
{
	/* ����Ѿ���¼���ͷ���״̬������Ҫ��ֹͣ�ٿ��� */
	g_tRec.ucStatus = STA_STOP_I2S;
	bsp_DelayMS(10);	/* �ӳ�һ��ʱ�䣬�ȴ�I2S�жϽ��� */
	wm8978_Init();		/* ��λWM8978����λ״̬ */
	I2S_Stop();			/* ֹͣI2S¼���ͷ��� */

	g_tRec.ucStatus = STA_RECORDING;		/* ¼��״̬ */

	g_tRec.uiCursor = 0;	/* ����ָ�븴λΪ0����ͷ��ʼ¼�� */

	/* ����WM8978оƬ������ΪMic�����Ϊ���� */
	//wm8978_CfgAudioPath(MIC_LEFT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	//wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* ���ڷ���������������ͬ���� */
	wm8978_SetEarVolume(g_tRec.ucVolume);

	/* ����MICͨ������ */
	wm8978_SetMicGain(g_tRec.ucMicGain);

	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	/* ����STM32��I2S��Ƶ�ӿ�(��������ֱ�׼I2S�ӿڣ�16bit�� 8K������), ��ʼ¼�� */
	I2S_StartRecord(g_FmtList[g_tRec.ucFmtIdx][0], g_FmtList[g_tRec.ucFmtIdx][1], g_FmtList[g_tRec.ucFmtIdx][2]);
}

/*
*********************************************************************************************************
*	�� �� ��: StopRec
*	����˵��: ֹͣ¼���ͷ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void StopRec(void)
{
	g_tRec.ucStatus = STA_IDLE;		/* ����״̬ */
	I2S_Stop();		/* ֹͣI2S¼���ͷ��� */
	wm8978_Init();	/* ��λWM8978����λ״̬ */
}

/*
*********************************************************************************************************
*	�� �� ��: I2S_CODEC_DataTransfer
*	����˵��: I2S���ݴ��亯��, ��SPI2 (I2S)�жϷ��������á�¼���ͷ������ڴ˺���������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void I2S_CODEC_DataTransfer(void)
{
	uint16_t usData;

	/* ֻ¼������(������STM32������MIC��װ��������), ����ʱ����������ͬ */
	if (g_tRec.ucStatus == STA_RECORDING)	/* ¼��״̬ */
	{
		if (SPI_I2S_GetFlagStatus(I2S2ext, SPI_I2S_FLAG_RXNE) == SET)
		{
			usData = SPI_I2S_ReceiveData(I2S2ext);
			g_tRec.pAudio[g_tRec.uiCursor] = usData;	/* ������Ƶ���� */
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
	else if (g_tRec.ucStatus == STA_PLAYING)	/* ����״̬����������ֵ���������� */
	{
		if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == SET)
		{
			#if 1		/* ˫ͨ������ */
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* ��ȡ��Ƶ���� */
				if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
				{
					g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
					/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
					SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
					/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
					SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

					bsp_PutKey(JOY_DOWN_OK);	/* ģ��ֹͣ������ */
				}
				SPI_I2S_SendData(SPI2, usData);
			#else	/* ���Խ�����1��ͨ�����������Ƶ�����1��ͨ��(����������¼����������¼��) */
				usData = g_tRec.pAudio[g_tRec.uiCursor];		/* ��ȡ��Ƶ���� */
				if (SPI_I2S_GetFlagStatus(SPI2, I2S_FLAG_CHSIDE) != SET)	/* �ж��ǲ��������� */
				{
					if (++g_tRec.uiCursor >= EXT_SRAM_SIZE / 2)
					{
						g_tRec.uiCursor = EXT_SRAM_SIZE / 2;
						/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
						SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
						/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
						SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);

						bsp_PutKey(JOY_DOWN_OK);	/* ģ��ֹͣ������ */
					}
				}
				SPI_I2S_SendData(SPI2, usData);
			#endif
		}
	}
	else 	/* ��ʱ״̬ STA_STOP_I2S������ֹͣ�ж� */
	{
		/* ��ֹI2S2 RXNE�ж�(���ղ���)����Ҫʱ�ٴ� */
		SPI_I2S_ITConfig(I2S2ext, SPI_I2S_IT_RXNE, DISABLE);
		/* ��ֹI2S2 TXE�ж�(���ͻ�������)����Ҫʱ�ٴ� */
		SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: SPI2_IRQHandler
*	����˵��: I2S���ݴ����жϷ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SPI2_IRQHandler(void)
{
	I2S_CODEC_DataTransfer();
}
