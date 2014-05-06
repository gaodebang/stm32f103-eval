/*
*********************************************************************************************************
*
*	ģ������ : �ⲿSRAM����ģ��
*	�ļ����� : bsp_fsmc_sram.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*********************************************************************************************************
*/

#ifndef _BSP_FSMC_SRAM_H
#define _BSP_FSMC_SRAM_H

#define EXT_SRAM_ADDR  	((uint32_t)0x68000000)
#define EXT_SRAM_SIZE	(2 * 1024 * 1024)

void bsp_InitExtSRAM(void);
uint8_t bsp_TestExtSRAM(void);

#endif
