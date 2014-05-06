/*
*********************************************************************************************************
*
*	模块名称 : 三轴磁力计HMC5883L驱动模块
*	文件名称 : bsp_HMC5883L.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_HMC5883L_H
#define _BSP_HMC5883L_H

#define HMC5883L_SLAVE_ADDRESS    0x3C		/* I2C从机地址 */

//****************************************
// 定义HMC5883L内部地址
//****************************************
#define DATA_OUT_X		0x03

typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Z;

	int16_t X_Min;
	int16_t Y_Min;
	int16_t Z_Min;

	int16_t X_Max;
	int16_t Y_Max;
	int16_t Z_Max;	

	uint8_t	Status;
	
	uint8_t CfgRegA;
	uint8_t CfgRegB;
	uint8_t CfgRegC;
	uint8_t ModeReg;
	
	uint8_t IDReg[3+1];	
}HMC5883L_T;

extern HMC5883L_T g_tMag;

void bsp_InitHMC5883L(void);

void HMC5883L_ReadData(void);
void HMC5883L_WriteByte(uint8_t _ucRegAddr, uint8_t _ucRegData);
uint8_t HMC5883L_ReadByte(uint8_t _ucRegAddr);

#endif
