#ifndef __fft_H
#define __fft_H


#define FFT_LEN 1024

extern int16_t aAudio[FFT_LEN];
extern int16_t aFft[FFT_LEN];


void Load_Data_FFT(int16_t dat[],int n);
void FFT(void);

#endif
