/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

#define EE_MODEL_NAME		"AT24C128"
#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
#define EE_SIZE				(16*1024)	/* ������(�ֽ�) */
#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */

uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif
