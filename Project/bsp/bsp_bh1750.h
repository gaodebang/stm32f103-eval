/*
*********************************************************************************************************
*
*	模块名称 : 光照度传感器BH1750FVI驱动模块
*	文件名称 : bsp_bh1750.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/
/*
	"1 - 连续高分辨率测量模式1，分辨率 1lux，典型测量时间 120ms (max 180ms)";
	"2 - 连续高分辨率测量模式2，分辨率 0.5lux，典型测量时间 120ms (max 180ms)";
	"3 - 连续低分辨率测量模式，分辨率 4lux，典型测量时间 16ms (max 24ms)";
	"4 - 设置最低灵敏度(1.85 lx 或 0.93 lx)， MT = 31";
	"5 - 设置最高灵敏度(0.23 lx 或 0.11 lx)， MT = 254";
	"6 - 设置缺省灵敏度(1.20 lx 或 0.60 lx)， MT = 69";
*/
#ifndef _BSP_BH1750_H
#define _BSP_BH1750_H

#define BH1750_SLAVE_ADDRESS    0x46		/* I2C从机地址 */
#define S_MODE_DEFAULT          2		    /* 默认测量模式 */
#define S_MTREG_DEFAULT         69		    /* 默认灵敏度倍率 */

/* 操作码 Opercode 定义 */
enum
{
	BHOP_POWER_DOWN = 0x00,		/* 进入掉电模式。芯片上电后缺省就是PowerDown模式 */
	BHOP_POWER_ON = 0x01,		/* 上电，等待测量命令 */
	BHOP_RESET = 0x07,			/* 清零数据寄存器 (Power Down 模式无效) */
	BHOP_CON_H_RES  = 0x10,		/* 连续高分辨率测量模式  （测量时间 120ms） （最大 180ms）*/
	BHOP_CON_H_RES2 = 0x11,		/* 连续高分辨率测量模式2 （测量时间 120ms）*/
	BHOP_CON_L_RES = 0x13,		/* 连续低分辨率测量模式 （测量时间 16ms）*/

	BHOP_ONE_H_RES  = 0x20,		/* 单次高分辨率测量模式 , 之后自动进入Power Down */
	BHOP_ONE_H_RES2 = 0x21,		/* 单次高分辨率测量模式2 , 之后自动进入Power Down  */
	BHOP_ONE_L_RES = 0x23,		/* 单次低分辨率测量模式 , 之后自动进入Power Down  */
};

extern float g_tBH1750;

void bsp_InitBH1750(void);
void BH1750_WriteCmd(uint8_t _ucOpecode);
uint16_t BH1750_ReadData(void);
void BH1750_AdjustSensitivity(uint8_t _ucMTReg);
void BH1750_ChageMode(uint8_t _ucMode);
float BH1750_GetLux(void);

#endif
