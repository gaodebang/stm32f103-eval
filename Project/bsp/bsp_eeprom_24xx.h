/*
*********************************************************************************************************
*
*	模块名称 : 串行EEPROM 24xx02驱动模块
*	文件名称 : bsp_eeprom_24xx.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

#define EE_MODEL_NAME		"AT24C128"
#define EE_DEV_ADDR			0xA0		/* 设备地址 */
#define EE_PAGE_SIZE		64			/* 页面大小(字节) */
#define EE_SIZE				(16*1024)	/* 总容量(字节) */
#define EE_ADDR_BYTES		2			/* 地址字节个数 */

uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif
