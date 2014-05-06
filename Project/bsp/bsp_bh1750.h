/*
*********************************************************************************************************
*
*	ģ������ : ���նȴ�����BH1750FVI����ģ��
*	�ļ����� : bsp_bh1750.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/
/*
	"1 - �����߷ֱ��ʲ���ģʽ1���ֱ��� 1lux�����Ͳ���ʱ�� 120ms (max 180ms)";
	"2 - �����߷ֱ��ʲ���ģʽ2���ֱ��� 0.5lux�����Ͳ���ʱ�� 120ms (max 180ms)";
	"3 - �����ͷֱ��ʲ���ģʽ���ֱ��� 4lux�����Ͳ���ʱ�� 16ms (max 24ms)";
	"4 - �������������(1.85 lx �� 0.93 lx)�� MT = 31";
	"5 - �������������(0.23 lx �� 0.11 lx)�� MT = 254";
	"6 - ����ȱʡ������(1.20 lx �� 0.60 lx)�� MT = 69";
*/
#ifndef _BSP_BH1750_H
#define _BSP_BH1750_H

#define BH1750_SLAVE_ADDRESS    0x46		/* I2C�ӻ���ַ */
#define S_MODE_DEFAULT          2		    /* Ĭ�ϲ���ģʽ */
#define S_MTREG_DEFAULT         69		    /* Ĭ�������ȱ��� */

/* ������ Opercode ���� */
enum
{
	BHOP_POWER_DOWN = 0x00,		/* �������ģʽ��оƬ�ϵ��ȱʡ����PowerDownģʽ */
	BHOP_POWER_ON = 0x01,		/* �ϵ磬�ȴ��������� */
	BHOP_RESET = 0x07,			/* �������ݼĴ��� (Power Down ģʽ��Ч) */
	BHOP_CON_H_RES  = 0x10,		/* �����߷ֱ��ʲ���ģʽ  ������ʱ�� 120ms�� ����� 180ms��*/
	BHOP_CON_H_RES2 = 0x11,		/* �����߷ֱ��ʲ���ģʽ2 ������ʱ�� 120ms��*/
	BHOP_CON_L_RES = 0x13,		/* �����ͷֱ��ʲ���ģʽ ������ʱ�� 16ms��*/

	BHOP_ONE_H_RES  = 0x20,		/* ���θ߷ֱ��ʲ���ģʽ , ֮���Զ�����Power Down */
	BHOP_ONE_H_RES2 = 0x21,		/* ���θ߷ֱ��ʲ���ģʽ2 , ֮���Զ�����Power Down  */
	BHOP_ONE_L_RES = 0x23,		/* ���εͷֱ��ʲ���ģʽ , ֮���Զ�����Power Down  */
};

extern float g_tBH1750;

void bsp_InitBH1750(void);
void BH1750_WriteCmd(uint8_t _ucOpecode);
uint16_t BH1750_ReadData(void);
void BH1750_AdjustSensitivity(uint8_t _ucMTReg);
void BH1750_ChageMode(uint8_t _ucMode);
float BH1750_GetLux(void);

#endif
