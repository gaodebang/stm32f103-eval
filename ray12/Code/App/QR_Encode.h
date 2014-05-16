#include "sys_config.h"



#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

#define QR_VRESION_S	0 
#define QR_VRESION_M	1 
#define QR_VRESION_L	2 

#define MAX_ALLCODEWORD	 3706 
#define MAX_DATACODEWORD 2956 
#define MAX_CODEBLOCK	  153 
#define MAX_MODULESIZE	  50	// 21:Version=1,����ַ�=17(8.5������)
															// 25:Version=2,����ַ�=32(16������)
															// 29:Version=3,����ַ�=49(24.5������)
															// 33:Version=4,����ַ�=78(39������)
															// 37:Version=5,����ַ�=106(53������) 
															// 41:Version=6,����ַ�=134(67������)
							 								// 45:Version=7,����ַ�=154(77������)
							 								// 49:Version=8,����ַ�=192(96������)
#define QR_MARGIN	4

extern BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE];


BOOL EncodeData(char *lpsSource);

int GetEncodeVersion(int nVersion, char *lpsSource, int ncLength);
BOOL EncodeSourceData(char *lpsSource, int ncLength, int nVerGroup);

int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

int SetBitStream(int nIndex, WORD wData, int ncData);

BOOL IsNumeralData(unsigned char c);
BOOL IsAlphabetData(unsigned char c);
BOOL IsKanjiData(unsigned char c1, unsigned char c2);

BYTE AlphabetToBinaly(unsigned char c);
WORD KanjiToBinaly(WORD wc);

void GetRSCodeWord(BYTE *lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

void FormatModule(void);

void SetFunctionModule(void);
void SetFinderPattern(int x, int y);
void SetAlignmentPattern(int x, int y);
void SetVersionPattern(void);
void SetCodeWordPattern(void);
void SetMaskingPattern(int nPatternNo);
void SetFormatInfoPattern(int nPatternNo);
int CountPenalty(void);
