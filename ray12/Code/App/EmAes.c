//=============================================================================
// Author: lxyppc
// Based on work by: shaoziyang  
// Base file: http://www.ourdev.cn/bbs/bbs_content.jsp?bbs_sn=1148131&bbs_page_no=1&search_mode=1&search_text=AES&bbs_id=9999
//
//=============================================================================
#include "includes.h"
#include "EmAES.h"


#define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).
#define BLOCKSIZE 16 //!< Block size in number of bytes.




/// Buffer used in the 
#ifdef    STM32
#define   emptyRAM    ((CPU_INT08U*)0x20001800) //((CPU_INT08U*)(__sfe( "CSTACK" ) + 64))

CPU_INT08U* block1 = emptyRAM;
CPU_INT08U* block2 = emptyRAM+256;
CPU_INT08U* tempbuf = emptyRAM+512;

#else
CPU_INT08U block1[256]; //!< Workspace 1.
CPU_INT08U block2[256]; //!< Worksapce 2.
CPU_INT08U tempbuf[256];
#endif

#define KEY_COUNT 1

#if KEY_COUNT == 1
  #define KEYBITS 128 //!< Use AES128.
#elif KEY_COUNT == 2
  #define KEYBITS 192 //!< Use AES196.
#elif KEY_COUNT == 3
  #define KEYBITS 256 //!< Use AES256.
#else
  #error Use 1, 2 or 3 keys!
#endif

#if KEYBITS == 128
  #define ROUNDS 10 //!< Number of rounds.
  #define KEYLENGTH 16 //!< Key length in number of bytes.
#elif KEYBITS == 192
  #define ROUNDS 12 //!< Number of rounds.
  #define KEYLENGTH 24 //!< // Key length in number of bytes.
#elif KEYBITS == 256
  #define ROUNDS 14 //!< Number of rounds.
  #define KEYLENGTH 32 //!< Key length in number of bytes.
#else
  #error Key must be 128, 192 or 256 bits!
#endif

CPU_INT08U kTable[16]
={
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
};
CPU_INT08U kTable2[16]
={
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
};
/*
#ifdef  STM32
;
#else
//!< Todo: change to your own key
={
 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
 0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
 0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
};
#endif
*/

CPU_INT08U *powTbl; //!< Final location of exponentiation lookup table.
CPU_INT08U *logTbl; //!< Final location of logarithm lookup table.
CPU_INT08U *sBox; //!< Final location of s-box.
CPU_INT08U *sBoxInv; //!< Final location of inverse s-box.
CPU_INT08U *expandedKey; //!< Final location of expanded key.

void CalcPowLog(CPU_INT08U *powTbl, CPU_INT08U *logTbl)
{
  CPU_INT08U i = 0;
  CPU_INT08U t = 1;

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



void CalcSBox( CPU_INT08U * sBox )
{
  CPU_INT08U i, rot;
  CPU_INT08U temp;
  CPU_INT08U result;

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



void CalcSBoxInv( CPU_INT08U * sBox, CPU_INT08U * sBoxInv )
{
  CPU_INT08U i = 0;
  CPU_INT08U j = 0;

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



void CycleLeft( CPU_INT08U * row )
{
  // Cycle 4 bytes in an array left once.
  CPU_INT08U temp = row[0];

  row[0] = row[1];
  row[1] = row[2];
  row[2] = row[3];
  row[3] = temp;
}


void CalcCols(CPU_INT08U *col)
{
  CPU_INT08U i;

  for(i = 4; i > 0; i--)
  {
    *col = (*col << 1) ^ (*col & 0x80 ? BPOLY : 0);
    col++;
  }
}

void InvMixColumn( CPU_INT08U * column )
{
  CPU_INT08U r[4];

  r[0] = column[1] ^ column[2] ^ column[3];
  r[1] = column[0] ^ column[2] ^ column[3];
  r[2] = column[0] ^ column[1] ^ column[3];
  r[3] = column[0] ^ column[1] ^ column[2];

  /*column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);*/
  CalcCols(column);

  r[0] ^= column[0] ^ column[1];
  r[1] ^= column[1] ^ column[2];
  r[2] ^= column[2] ^ column[3];
  r[3] ^= column[0] ^ column[3];

  /*column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);*/
  CalcCols(column);

  r[0] ^= column[0] ^ column[2];
  r[1] ^= column[1] ^ column[3];
  r[2] ^= column[0] ^ column[2];
  r[3] ^= column[1] ^ column[3];

  /*column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);
  column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);
  column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);
  column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);*/
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



void SubBytes( CPU_INT08U * bytes, CPU_INT32U count )
{
  do {
    *bytes = sBox[ *bytes ]; // Substitute every byte in state.
    bytes++;
  } while( --count );
}



void InvSubBytesAndXOR( CPU_INT08U * bytes, CPU_INT08U * key, CPU_INT08U count )
{
  do {
    // *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.
    *bytes = block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.
    bytes++;
    key++;
  } while( --count );
}


void InvShiftRows( CPU_INT08U * state )
{
  CPU_INT08U temp;

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
void InvMixColumns( CPU_INT08U * state )
{
  InvMixColumn( state + 0*4 );
  InvMixColumn( state + 1*4 );
  InvMixColumn( state + 2*4 );
  InvMixColumn( state + 3*4 );
}
*/

#ifdef	STM32
void XORBytes32( CPU_INT32U * bytes1, CPU_INT32U * bytes2, CPU_INT32U count )
{
  do {
    *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
    bytes1++;
    bytes2++;
  } while( --count );
}

#define		XORBytes(b1,b2,cnt)		\
	XORBytes32((CPU_INT32U*)(b1),(CPU_INT32U*)(b2),(CPU_INT32U)(cnt/sizeof(int)) )

void CopyBytes32( CPU_INT32U * to, CPU_INT32U * from, CPU_INT32U count )
{
  do {
    *to = *from;
    to++;
    from++;
  } while( --count );
}

#define		CopyBytes(to,from,cnt)		\
	CopyBytes32((CPU_INT32U*)(to),(CPU_INT32U*)(from),(CPU_INT32U)(cnt/sizeof(int)) )


#else /// STM32

void XORBytes( CPU_INT08U * bytes1, CPU_INT08U * bytes2, CPU_INT08U count )
{
  do {
    *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.
    bytes1++;
    bytes2++;
  } while( --count );
}

void CopyBytes( CPU_INT08U * to, CPU_INT08U * from, CPU_INT08U count )
{
  do {
    *to = *from;
    to++;
    from++;
  } while( --count );
}

#endif  /// STM32



void KeyExpansion( CPU_INT08U * expandedKey )
{
  CPU_INT08U temp[4];
  CPU_INT08U i;
  CPU_INT08U Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.

  const CPU_INT08U * key = kTable;

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



void InvCipher( CPU_INT08U * block, CPU_INT08U * expandedKey )
{
  CPU_INT08U i, j;
  CPU_INT08U round = ROUNDS-1;
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

void aesDecrypt( CPU_INT08U * buffer, CPU_INT08U * chainBlock )
{
  CopyBytes( tempbuf, buffer, BLOCKSIZE );
  InvCipher( buffer, expandedKey );
  XORBytes( buffer, chainBlock, BLOCKSIZE );
  CopyBytes( chainBlock, tempbuf, BLOCKSIZE );
}

CPU_INT08U Multiply( CPU_INT08U num, CPU_INT08U factor )
{
  CPU_INT08U mask = 1;
  CPU_INT08U result = 0;

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


CPU_INT08U DotProduct( const CPU_INT08U * vector1, CPU_INT08U * vector2 )
{
  CPU_INT08U result = 0 ,i;

  //result ^= Multiply( *vector1++, *vector2++ );
  //result ^= Multiply( *vector1++, *vector2++ );
  //result ^= Multiply( *vector1++, *vector2++ );
  //result ^= Multiply( *vector1  , *vector2   );

  for(i = 4; i > 0; i--)
    result ^= Multiply( *vector1++, *vector2++ );

  return result;
}


void MixColumn( CPU_INT08U * column )
{
  const CPU_INT08U row[8] = {
    0x02, 0x03, 0x01, 0x01,
    0x02, 0x03, 0x01, 0x01
  }; // Prepare first row of matrix twice, to eliminate need for cycling.

  CPU_INT08U result[4];

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
/*
void MixColumns( CPU_INT08U * state )
{
  MixColumn( state + 0*4 );
  MixColumn( state + 1*4 );
  MixColumn( state + 2*4 );
  MixColumn( state + 3*4 );
}
*/
void ShiftRows( CPU_INT08U * state )
{
  CPU_INT08U temp;

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


void Cipher( CPU_INT08U * block, CPU_INT08U * expandedKey )
{
  CPU_INT08U i, j;
  CPU_INT08U round = ROUNDS-1;

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

void aesEncrypt( CPU_INT08U * buffer, CPU_INT08U * chainBlock )
{
  XORBytes( buffer, chainBlock, BLOCKSIZE );
  Cipher( buffer, expandedKey );
  CopyBytes( chainBlock, buffer, BLOCKSIZE );
}





/*
////////////////////////////////////////////////////////////////////
// The STM32 has a hardware CRC modual
//  Refered in RM0008
//
// Uses CRC-32 (Ethernet) polynomial: 0x4C11DB7
// X32 + X25 + X23 + X22 + X16 + X12 + X10 + X11 +X8 + X7 + X5 + X4 + X2+ X +1
// The reverse poly is 0xEDB88320
////////////////////////////////////////////////////////////////////

#ifndef	STM32
CPU_INT32U Crc32Table[256];
#define		CRC32_MASK		((CPU_INT32U)0xFFFFFFFF)

void MakeCrc32Table()
{
    CPU_INT32U i,j;
    CPU_INT32U crc;
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

CPU_INT32U  CalcCrc32(const CPU_INT08U* data, CPU_INT32U size)
{
	CPU_INT32U crc = CRC32_MASK;
	while(size--){
		crc = (crc << 8) ^ Crc32Table[ ((crc >> (32-8)) & 0xFF) ^ *data++]; 
	}
	crc ^= CRC32_MASK;
	return crc;
}


void MakeCrc32TableRev()
{
    CPU_INT32U i,j;
    CPU_INT32U crc;
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

CPU_INT32U  CalcCrc32Rev(const CPU_INT08U* data, CPU_INT32U size)
{
	CPU_INT32U crc = CRC32_MASK;
	while(size--){
		crc = (crc >> 8) ^ Crc32Table[ (crc & 0xFF) ^ *data++]; 
	}
	crc ^= CRC32_MASK;
	return crc;
}

CPU_INT32U CrcGen_STM32(CPU_INT32U data[], CPU_INT32U size)
{
  CPU_INT32U crc = 0xFFFFFFFF;
  for(CPU_INT32U i=0;i<size;i++){
    CPU_INT32U wdata = data[i];
    for(CPU_INT32U j=0;j<32;j++){
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
*/