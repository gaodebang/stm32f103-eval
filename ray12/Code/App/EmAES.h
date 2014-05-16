//=============================================================================
// Author: lxyppc
// Based on work by: shaoziyang  
// Base file: http://www.ourdev.cn/bbs/bbs_content.jsp?bbs_sn=1148131&bbs_page_no=1&search_mode=1&search_text=AES&bbs_id=9999
//
//=============================================================================

#ifndef   EMAES_H
#define   EMAES_H

#include "sys_config.h"

/// For stm32, place the template buffer to the unreachable memory
extern u8 *sBox; //!< Final location of s-box.
extern u8 *sBoxInv; //!< Final location of inverse s-box.

/// For other platform, just allocate in the stack memory
extern u8 block1[ 256 ]; //!< Workspace 1.
extern u8 block2[ 256 ]; //!< Worksapce 2.
extern u8 tempbuf[256];

void aesEncrypt( u8 * buffer, u8 * chainBlock );
void aesEncInit( void );
void aesDecrypt( u8 * buffer, u8 * chainBlock );
void aesDecInit( void );
void Data_Encrypt(u8* data,u8* dest,u8 length);
void Data_Decrypt(u8* data,u8* dest,u8 length);


extern u32 Crc32Table[256];


void MakeCrc32Table();
u32  CalcCrc32(const u8* data, u32 size);
void MakeCrc32TableRev();
u32  CalcCrc32Rev(const u8* data, u32 size);

u32 CrcGen_STM32(u32 data[], u32 size);

#endif

