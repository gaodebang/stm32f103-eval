/*
*********************************************************************************************************
*
*	模块名称 : 外部SRAM驱动模块
*	文件名称 : bsp_fsmc_sram.h
*	版    本 : V1.0
*	说    明 : 头文件
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
