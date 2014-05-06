
#include "app.h"
#include <math.h>
#include <inttypes.h>

#define PI 3.1415926
			
float AI[FFT_LEN],AR[FFT_LEN];
int16_t aAudio[FFT_LEN];
int16_t aFft[FFT_LEN];

void Load_Data_FFT(int16_t dat[],int n)
{
	int i;
	for (i=0;i<n;i++)
	{
		AR[i]=(float)dat[i];
		AI[i]=0;
	}
}

void DFT_FFT (int m,int n)
{
	int i,i1,j1,j,LH,n1,k,b;
	float tr,ti,p;
	LH=(int)n/2;
	j=LH;
	n1=n-2;
	for(i=1;i<=n1;i++)
	{
		if(i<j)
		{
			i1=i;j1=j;
			tr=AR[i1];
			ti=AI[i1];
			AR[i1]=AR[j1];
			AI[i1]=AI[j1];
			AR[j1]=tr;
			AI[j1]=ti;
		}
		k=LH;
		while(j>=k)
		{
			j=j-k;
			k=(int)k/2;
		}
		j=j+k;
	}
		
	for(i=0;i<=m;i++)
	{
		b=(int)pow(2.0,(i-1));     
		for(j=0;j<=(b-1);j++)
		{
			p=(float)(pow(2.0,(m-i))*j*2.0*PI/(float)n);
			for(k=j;k<=(n-1);)
			{
				tr=(float)(AR[k+b]*cos(p)+AI[k+b]*sin(p));
				ti=(float)(AI[k+b]*cos(p)-AR[k+b]*sin(p));
				AR[k+b]=AR[k]-tr;
				AI[k+b]=AI[k]-ti;
				AR[k]=AR[k]+tr;
				AI[k]=AI[k]+ti;
				k+=b*2;
			}
		}
	}
}

void Sum_Squre_FFT(void)
{
    uint32_t i;
	for (i=0;i<FFT_LEN;i++)
	{
		aFft[i]=(int)((AI[i]*AI[i]+AR[i]*AR[i])/65536);
	}
}

void FFT(void)
{
	DFT_FFT(10,FFT_LEN);
	Sum_Squre_FFT();
}
