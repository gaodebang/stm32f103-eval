/*
*********************************************************************************************************
*
*	模块名称 : 录音演示程序
*	文件名称 : audio_rec.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __audio_rec_H
#define __audio_rec_H


/* 录音机状态 */
enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_RECORDING,	/* 录音状态 */
	STA_PLAYING,	/* 放音状态 */
	STA_STOP_I2S,	/* 临时状态，主程序通知I2S中断服务程序准备停止 */
};

typedef struct
{
	uint8_t ucFmtIdx;			/* 音频格式：标准、位长、采样频率 */
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucMicGain;			/* 当前MIC增益 */
	int16_t *pAudio;			/* 定义一个指针，用于访问外部SRAM */
	uint32_t uiCursor;			/* 播放位置 */

	uint8_t ucSpkOutEn;			/* 放音时，是否打开扬声器 */

	uint8_t ucStatus;			/* 录音机状态，0表示待机，1表示录音中，2表示播放中 */
}REC_T;

void RecorderDemo(void);

#endif

