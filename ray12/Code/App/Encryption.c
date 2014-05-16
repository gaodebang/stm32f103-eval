#include "EmAES.h"

#define MAXSIZE  32
#define BPOLY 0x1b     //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16   //!< Block size in number of bytes.


u8 block1[256];   //!< Workspace 1.
u8 block2[256];   //!< Worksapce 2.
u8 tempbuf[256];

#define KEY_COUNT 1

#if KEY_COUNT == 1
  #define KEYBITS 128   //!< Use AES128.
#elif KEY_COUNT == 2
  #define KEYBITS 192   //!< Use AES196.
#elif KEY_COUNT == 3
  #define KEYBITS 256   //!< Use AES256.
#else
  #error Use 1, 2 or 3 keys!
#endif

#if KEYBITS == 128
  #define ROUNDS 10    //!< Number of rounds.
  #define KEYLENGTH 16 //!< Key length in number of bytes.
#elif KEYBITS == 192
  #define ROUNDS 12    //!< Number of rounds.
  #define KEYLENGTH 24 //!< // Key length in number of bytes.
#elif KEYBITS == 256
  #define ROUNDS 14    //!< Number of rounds.
  #define KEYLENGTH 32 //!< Key length in number of bytes.
#else
  #error Key must be 128, 192 or 256 bits!
#endif

u8 kTable[16]   //加密密钥
={
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
};


u8 *powTbl;   //!< Final location of exponentiation lookup table.
u8 *logTbl;   //!< Final location of logarithm lookup table.
u8 *sBox;     //!< Final location of s-box.
u8 *sBoxInv;  //!< Final location of inverse s-box.
u8 *expandedKey; //!< Final location of expanded key.


//void print(u8* ch,unsigned int length);


//CalcPowLog()与CalcSBox()共同产生sbox
void CalcPowLog(u8 *powTbl, u8 *logTbl)
{
  u8 i = 0;
  u8 t = 1;
  do {
    // Use 0x03 as root for exponentiation and logarithms.
    powTbl[i] = t;
    logTbl[t] = i;
    i++;

    // Muliply t by 3 in GF(2^8).
    t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
  }while( t != 1 ); // Cyclic properties ensure that i < 255.

  powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.
  
}



void CalcSBox( u8 * sBox )
{
  u8 i, rot;
  u8 temp;
  u8 result;

  // Fill all entries of sBox[].
  i = 0;
  do {
    //Inverse in GF(2^8).
    if( i > 0 ) 
    {
      temp = powTbl[ 255 - logTbl[i] ];
    } 
    else 
    {
      temp = 0;
    }

    // Affine transformation in GF(2).
    result = temp ^ 0x63; // Start with adding a vector in GF(2).
    for( rot = 4; rot > 0; rot-- )
    {
      // Rotate left.
      temp = (temp<<1) | (temp>>7);

      // Add rotated byte in GF(2).
      result ^= temp;
    }

    // Put result in table.
    sBox[i] = result;
  } while( ++i != 0 );
}


//字节替换
void SubBytes( u8 * bytes, u32 count )
{
  do {
    *bytes = sBox[ *bytes ]; // Substitute every byte in state.
    bytes++;
  } while( --count );
}

//左移一位
void CycleLeft( u8 * row )
{
  // Cycle 4 bytes in an array left once.
  u8 temp = row[0];

  row[0] = row[1];
  row[1] = row[2];
  row[2] = row[3];
  row[3] = temp;
}





//XORBytes() equal AddRoundKey()
#ifdef	STM32
void XORBytes32( u32 * bytes1, u32 * bytes2, u32 count )
{
  do {
    *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
    bytes1++;
    bytes2++;
  } while( --count );
}

#define		XORBytes(b1,b2,cnt)		\
	XORBytes32((u32*)(b1),(u32*)(b2),(u32)(cnt/sizeof(int)) )

void CopyBytes32( u32 * to, u32 * from, u32 count )
{
  do {
    *to = *from;
    to++;
    from++;
  } while( --count );
}

#define		CopyBytes(to,from,cnt)		\
	CopyBytes32((u32*)(to),(u32*)(from),(u32)(cnt/sizeof(int)) )


#else /// STM32

void XORBytes( u8 * bytes1, u8 * bytes2, u8 count )
{
  do {
    *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
    bytes1++;
    bytes2++;
  } while( --count );
}

void CopyBytes( u8 * to, u8 * from, u8 count )
{
  do {
    *to = *from;
    to++;
    from++;
  } while( --count );
}

#endif  /// STM32


//计算expandedkey
void KeyExpansion( u8 * expandedKey )
{
  u8 temp[4];
  u8 i;
  u8 Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.

  const u8 * key = kTable;    //初始密钥

  // Copy key to start of expanded key.
  i = KEYLENGTH;
  do {
    *expandedKey = *key;
    expandedKey++;
    key++;
  } while( --i );

  // Prepare last 4 bytes of key in temp.
  /*expandedKey -= 4;
  temp[0] = *(expandedKey++);
  temp[1] = *(expandedKey++);
  temp[2] = *(expandedKey++);
  temp[3] = *(expandedKey++);*/
  CopyBytes(temp, expandedKey-4, 4);

  // Expand key.
  i = KEYLENGTH;
  //j = BLOCKSIZE*(ROUNDS+1) - KEYLENGTH;
  while( i < BLOCKSIZE*(ROUNDS+1) ) 
  {
    // Are we at the start of a multiple of the key size?
    if( (i % KEYLENGTH) == 0 )
    {
      CycleLeft( temp ); // Cycle left once.
      SubBytes( temp, 4 ); // Substitute each byte.
      XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).
      *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
    }

    // Keysize larger than 24 bytes, ie. larger that 192 bits?
    #if KEYLENGTH > 24
    // Are we right past a block size?
    else if( (i % KEYLENGTH) == BLOCKSIZE ) {
      SubBytes( temp, 4 ); // Substitute each byte.
    }
    #endif

    // Add bytes in GF(2) one KEYLENGTH away.
    XORBytes( temp, expandedKey - KEYLENGTH, 4 );

    // Copy result to current 4 bytes.
    *(expandedKey++) = temp[ 0 ];
    *(expandedKey++) = temp[ 1 ];
    *(expandedKey++) = temp[ 2 ];
    *(expandedKey++) = temp[ 3 ];
    //CopyBytes(expandedKey, temp, 4);
    //expandedKey += 4;

    i += 4; // Next 4 bytes.
  }
}





u8 Multiply( u8 num, u8 factor )
{
  u8 mask = 1;
  u8 result = 0;

  while( mask != 0 ) 
  {
    // Check bit of factor given by mask.
    if( mask & factor ) 
    {
      // Add current multiple of num in GF(2).
      result ^= num;
    }

    // Shift mask to indicate next bit.
    mask <<= 1;

    // Double num.
    num = (num << 1) ^ (num & 0x80 ? BPOLY : 0);
  }

  return result;
}


u8 DotProduct( const u8 * vector1, u8 * vector2 )
{
  u8 result = 0 ,i;

  //result ^= Multiply( *vector1++, *vector2++ );
  //result ^= Multiply( *vector1++, *vector2++ );
  //result ^= Multiply( *vector1++, *vector2++ );
  //result ^= Multiply( *vector1  , *vector2   );

  for(i = 4; i > 0; i--)
    result ^= Multiply( *vector1++, *vector2++ );

  return result;
}


void MixColumn( u8 * column )
{
  const u8 row[8] = {
    0x02, 0x03, 0x01, 0x01,
    0x02, 0x03, 0x01, 0x01
  }; // Prepare first row of matrix twice, to eliminate need for cycling.

  u8 result[4];

  // Take dot products of each matrix row and the column vector.
  result[0] = DotProduct( row+0, column );
  result[1] = DotProduct( row+3, column );
  result[2] = DotProduct( row+2, column );
  result[3] = DotProduct( row+1, column );

  // Copy temporary result to original column.
  //column[0] = result[0];
  //column[1] = result[1];
  //column[2] = result[2];
  //column[3] = result[3];
  CopyBytes(column, result, 4);
}

void ShiftRows( u8 * state )
{
  u8 temp;

  // Note: State is arranged column by column.

  // Cycle second row left one time.
  temp = state[ 1 + 0*4 ];
  state[ 1 + 0*4 ] = state[ 1 + 1*4 ];
  state[ 1 + 1*4 ] = state[ 1 + 2*4 ];
  state[ 1 + 2*4 ] = state[ 1 + 3*4 ];
  state[ 1 + 3*4 ] = temp;

  // Cycle third row left two times.
  temp = state[ 2 + 0*4 ];
  state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
  state[ 2 + 2*4 ] = temp;
  temp = state[ 2 + 1*4 ];
  state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
  state[ 2 + 3*4 ] = temp;

  // Cycle fourth row left three times, ie. right once.
  temp = state[ 3 + 3*4 ];
  state[ 3 + 3*4 ] = state[ 3 + 2*4 ];
  state[ 3 + 2*4 ] = state[ 3 + 1*4 ];
  state[ 3 + 1*4 ] = state[ 3 + 0*4 ];
  state[ 3 + 0*4 ] = temp;
}

//加密函数
void Cipher( u8 * block, u8 * expandedKey )
{
  u8 i, j;
  u8 round = ROUNDS-1;

  XORBytes( block, expandedKey, 16 );
  expandedKey += BLOCKSIZE;

  do {
    SubBytes( block, 16 );
    ShiftRows( block );
    //MixColumns( block );
    for(i = 4, j = 0; i > 0; i--, j+=4)
      MixColumn( block + j );
    XORBytes( block, expandedKey, 16 );
    expandedKey += BLOCKSIZE;
  } while( --round );

  SubBytes( block, 16 );
  ShiftRows( block );
  XORBytes( block, expandedKey, 16 );
}

void aesEncInit(void)
{
  powTbl = block1;
  logTbl = block2;
  CalcPowLog( powTbl, logTbl );

  sBox = block2;
  CalcSBox( sBox );

  expandedKey = block1;
  KeyExpansion( expandedKey );
}

void aesEncrypt( u8 * buffer, u8 * chainBlock )
{
  //XORBytes( buffer, chainBlock, BLOCKSIZE );

  Cipher( buffer, expandedKey );
  //CopyBytes( chainBlock, buffer, BLOCKSIZE );
}


void CalcSBoxInv( u8 * sBox, u8 * sBoxInv )
{
  u8 i = 0;
  u8 j = 0;

  // Iterate through all elements in sBoxInv using  i.
  do {
    // Search through sBox using j.
    do {
      // Check if current j is the inverse of current i.
      if( sBox[ j ] == i )
      {
        // If so, set sBoxInc and indicate search finished.
        sBoxInv[ i ] = j;
        j = 255;
      }
    } while( ++j != 0 );
  } while( ++i != 0 );
}




void CalcCols(u8 *col)
{
  u8 i;

  for(i = 4; i > 0; i--)
  {
    *col = (*col << 1) ^ (*col & 0x80 ? BPOLY : 0);
    col++;
  }
}

void InvMixColumn( u8 * column )
{
  u8 r[4];

  r[0] = column[1] ^ column[2] ^ column[3];
  r[1] = column[0] ^ column[2] ^ column[3];
  r[2] = column[0] ^ column[1] ^ column[3];
  r[3] = column[0] ^ column[1] ^ column[2];


  CalcCols(column);

  r[0] ^= column[0] ^ column[1];
  r[1] ^= column[1] ^ column[2];
  r[2] ^= column[2] ^ column[3];
  r[3] ^= column[0] ^ column[3];


  CalcCols(column);

  r[0] ^= column[0] ^ column[2];
  r[1] ^= column[1] ^ column[3];
  r[2] ^= column[0] ^ column[2];
  r[3] ^= column[1] ^ column[3];

  
  CalcCols(column);

  column[0] ^= column[1] ^ column[2] ^ column[3];
  r[0] ^= column[0];
  r[1] ^= column[0];
  r[2] ^= column[0];
  r[3] ^= column[0];
  
  column[0] = r[0];
  column[1] = r[1];
  column[2] = r[2];
  column[3] = r[3];

  //CopyBytes(column, r, 4);
}






void InvSubBytesAndXOR( u8 * bytes, u8 * key, u8 count )
{
  do {
    // *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add 

//key.
    *bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
    bytes++;
    key++;
  } while( --count );
}


void InvShiftRows( u8 * state )
{
  u8 temp;

  // Note: State is arranged column by column.

  // Cycle second row right one time.
  temp = state[ 1 + 3*4 ];
  state[ 1 + 3*4 ] = state[ 1 + 2*4 ];
  state[ 1 + 2*4 ] = state[ 1 + 1*4 ];
  state[ 1 + 1*4 ] = state[ 1 + 0*4 ];
  state[ 1 + 0*4 ] = temp;

  // Cycle third row right two times.
  temp = state[ 2 + 0*4 ];
  state[ 2 + 0*4 ] = state[ 2 + 2*4 ];
  state[ 2 + 2*4 ] = temp;
  temp = state[ 2 + 1*4 ];
  state[ 2 + 1*4 ] = state[ 2 + 3*4 ];
  state[ 2 + 3*4 ] = temp;

  // Cycle fourth row right three times, ie. left once.
  temp = state[ 3 + 0*4 ];
  state[ 3 + 0*4 ] = state[ 3 + 1*4 ];
  state[ 3 + 1*4 ] = state[ 3 + 2*4 ];
  state[ 3 + 2*4 ] = state[ 3 + 3*4 ];
  state[ 3 + 3*4 ] = temp;
}


/*
void InvMixColumns( u8 * state )
{
  InvMixColumn( state + 0*4 );
  InvMixColumn( state + 1*4 );
  InvMixColumn( state + 2*4 );
  InvMixColumn( state + 3*4 );
}
*/

void InvCipher( u8 * block, u8 * expandedKey )
{
  u8 i, j;
  u8 round = ROUNDS-1;
  expandedKey += BLOCKSIZE * ROUNDS;

  XORBytes( block, expandedKey, 16 );
  expandedKey -= BLOCKSIZE;

  do {
    InvShiftRows( block );
    InvSubBytesAndXOR( block, expandedKey, 16 );
    expandedKey -= BLOCKSIZE;
    //InvMixColumns( block );
    for(i = 4, j = 0; i > 0; i--, j+=4)
      InvMixColumn( block + j );
  } while( --round );

  InvShiftRows( block );
  InvSubBytesAndXOR( block, expandedKey, 16 );
}


void aesDecInit(void)
{
  powTbl = block1;
  logTbl = block2;
  CalcPowLog( powTbl, logTbl );

  sBox = tempbuf;
  CalcSBox( sBox );

  expandedKey = block1;
  KeyExpansion( expandedKey );

  sBoxInv = block2; // Must be block2.
  CalcSBoxInv( sBox, sBoxInv );
}


//解密
void aesDecrypt( u8 * buffer, u8 * chainBlock )
{
  //CopyBytes( tempbuf, buffer, BLOCKSIZE );
  InvCipher( buffer, expandedKey );
  //XORBytes( buffer, chainBlock, BLOCKSIZE );
  //CopyBytes( chainBlock, tempbuf, BLOCKSIZE );
}



////////////////////////////////////////////////////////////////////
// The STM32 has a hardware CRC modual
//  Refered in RM0008
//
// Uses CRC-32 (Ethernet) polynomial: 0x4C11DB7
// X32 + X25 + X23 + X22 + X16 + X12 + X10 + X11 +X8 + X7 + X5 + X4 + X2+ X +1
// The reverse poly is 0xEDB88320
////////////////////////////////////////////////////////////////////

#ifndef	STM32
u32 Crc32Table[256];
#define		CRC32_MASK		((u32)0xFFFFFFFF)

void MakeCrc32Table()
{
    u32 i,j;
    u32 crc;
    for (i = 0; i < 256; i++){
        crc = i<<24;
        for (j = 0; j < 8; j++){
			if (crc & 0x80000000){
                crc = (crc << 1) ^ 0x4C11DB7;
			}else{
                crc <<= 1;
			}
        }
        Crc32Table[i] = crc&CRC32_MASK;
    }
}

u32  CalcCrc32(const u8* data, u32 size)
{
	u32 crc = CRC32_MASK;
	while(size--){
		crc = (crc << 8) ^ Crc32Table[ ((crc >> (32-8)) & 0xFF) ^ *data++]; 
	}
	crc ^= CRC32_MASK;
	return crc;
}


void MakeCrc32TableRev()
{
    u32 i,j;
    u32 crc;
    for (i = 0; i < 256; i++){
        crc = i;
        for (j = 0; j < 8; j++){
			if (crc & 0x01){
                crc = (crc >> 1) ^ 0xEDB88320;
			}else{
                crc >>= 1;
			}
        }
        Crc32Table[i] = crc&CRC32_MASK;
    }
}

u32  CalcCrc32Rev(const u8* data, u32 size)
{
	u32 crc = CRC32_MASK;
	while(size--){
		crc = (crc >> 8) ^ Crc32Table[ (crc & 0xFF) ^ *data++]; 
	}
	crc ^= CRC32_MASK;
	return crc;
}

u32 CrcGen_STM32(u32 data[], u32 size)
{
  u32 crc = 0xFFFFFFFF;
  for(u32 i=0;i<size;i++){
    u32 wdata = data[i];
    for(u32 j=0;j<32;j++){
      if( (crc ^ wdata) & 0x80000000 ){
        crc = 0x04C11DB7 ^ (crc<<1);
      }else{
        crc <<=1;
      }
      wdata<<=1;
    }
  }
  return crc;
}

#endif

//数据加密
void Data_Encrypt(u8* data,u8* dest,u8 length)
{
   u8 Aes_Data[16];
   
   //aesEncInit();
   
   for(u8 i = 0;i < 16;i++)
   {
    if(i < length)
      Aes_Data[i] = data[i];
    else
      Aes_Data[i] = i*(i%10);
   }
   aesEncrypt(Aes_Data, kTable);  //加密
  
   for(u8 i = 0;i < 16;i++)
       dest[i] = Aes_Data[i];
}

void Data_Decrypt(u8* data,u8* dest,u8 length)
{
  u8 Aes_Data[16];
   
   for(u8 i = 0;i < 16;i++)
    if(i < length)
      Aes_Data[i] = data[i];
    else
      Aes_Data[i] = 0;
   
   aesDecrypt(Aes_Data, kTable);  //加密
  
   for(u8 i = 0;i < length;i++)
       dest[i] = Aes_Data[i];
}








