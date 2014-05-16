#include "rtc.h"

u8 Time[8] = {0};   //����ʱ�䣬����������ʱ��

tm timer;  //�ṹ�壬���������ʱ����


//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//����:���
//���:������ǲ�������.1,��.0,����
static u8 Is_Leap_Year(u16 year)
{                    
		return (year%(year%100 ? 4:400) ? 0:1);
}     

//�·����ݱ�                                                                       
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
void RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
	    
    if(syear<2000||syear>2099)
    {
      return ;//syear��Χ1970-2099���˴����÷�ΧΪ2000-2099
    }
          
    for(t=1970;t<syear;t++) //��������ݵ��������
    {
        if(Is_Leap_Year(t))
        {
          seccount+=31622400;//�����������
        }
        else 
        {
          seccount+=31536000;                    //ƽ���������
        }
    }
    smon-=1;
    for(t=0;t<smon;t++)         //��ǰ���·ݵ����������
    {
        seccount+=(u32) mon_table[t]*86400;//�·����������
       
        if(t==1 && Is_Leap_Year(syear))
        {
          seccount+=86400;					//����2�·�����һ���������
        }
    }
    
    seccount+=((u32)sday-(u32)1)*(u32)86400;	//��ǰ�����ڵ����������
    seccount+=(u32)hour*(u32)3600;				//Сʱ������
    seccount+=(u32)min*(u32)60;      		//����������
    seccount+=(u32)sec;									//�������Ӽ���ȥ
                                                
    RTC_SetCounter(seccount);   		//����������Ĵ���
		//�ȴ��������
		RTC_WaitForLastTask();
}
void RTC_Set_Time(u8 *time)
{
	u16 syear;
	u8 smon, sday, hour, min, sec;
	syear=(u16)((time[0]&0xf0)>>4)*1000+(u16)(time[0]&0xf)*100+(u16)((time[1]&0xf0)>>4)*10+(u16)(time[1]&0xf);
	smon =(u16)((time[2]&0xf0)>>4)*10+(u16)(time[2]&0xf);
	sday =(u16)((time[3]&0xf0)>>4)*10+(u16)(time[3]&0xf);
	hour =(u16)((time[4]&0xf0)>>4)*10+(u16)(time[4]&0xf);
	min  =(u16)((time[5]&0xf0)>>4)*10+(u16)(time[5]&0xf);
	sec  =(u16)((time[6]&0xf0)>>4)*10+(u16)(time[6]&0xf);
  //����BKP��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
  //�������
  PWR_BackupAccessCmd(ENABLE);	
	RTC_Set(syear, smon, sday, hour, min, sec);
  //�ر�BKP��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, DISABLE);
  //��������
  PWR_BackupAccessCmd(DISABLE);
}
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//�������������������
//����ֵ�����ں�                                                                                                                                                    
static u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{    
       u16 temp2;
       u8 yearH,yearL;
      
       yearH=year/100;     yearL=year%100;
       // ���Ϊ21����,�������100 
       if (yearH>19)yearL+=100;
       // ����������ֻ��1900��֮��� 
       temp2=yearL+yearL/4;
       temp2=temp2%7;
       temp2=temp2+day+table_week[month-1];
       if (yearL%4==0&&month<3)temp2--;
       return(temp2%7);
}

static void TimeDate_Display(void)
{
  for(u8 i = 0;i < 8;i++)
  {Time[i] = 0;}
  Time[0] |= ((timer.w_year / 1000 ) << 4);
  Time[0] |= ((timer.w_year % 1000 / 100) << 0);
  Time[1] |= ((timer.w_year % 100 / 10) << 4);
  Time[1] |= ((timer.w_year % 10) << 0);
  Time[2] |= ((timer.w_month /10) << 4);
  Time[2] |= ((timer.w_month % 10) << 0);
  Time[3] |= ((timer.w_date / 10) << 4);
  Time[3] |= ((timer.w_date % 10) << 0);
  Time[4] |= ((timer.hour / 10) << 4);
  Time[4] |= ((timer.hour % 10) << 0);
  Time[5] |= ((timer.min / 10) << 4);
  Time[5] |= ((timer.min % 10) << 0);
  Time[6] |= ((timer.sec / 10) << 4);
  Time[6] |= ((timer.sec % 10) << 0);
  Time[7]  = timer.w_date % 10;
}

//�õ���ǰ��ʱ��
void RTC_Get(void)
{
    static u32 daycnt=0;
    u32 timecount=0;
    u32 temp  = 0;
    u32 temp1 = 0;
 
    timecount = RTC_GetCounter();      
 
    temp=timecount/86400;   //�õ�����(��������Ӧ��)
    if(daycnt!=temp)//����һ����
    {      
       daycnt=temp;
       temp1=1970;  //��1970�꿪ʼ
       while(temp>=365)
       {                         
         if(Is_Leap_Year(temp1))//������
          {
             if(temp>=366)
             {
               temp-=366;//�����������
             }
             else
             {
               temp1++;
               break;
             } 
          }
         else 
         {
            temp-=365;       //ƽ��
         }
          temp1++; 
        }  

        timer.w_year=temp1;//�õ����
        temp1=0;
        while(temp>=28)//������һ����
        {
          if(temp1==1 && Is_Leap_Year(timer.w_year))//�����ǲ�������/2�·�
         {
           if(temp>=29)
            
             temp-=29;//�����������
  
           else
             break;
          }
         else
         {
           if(temp>=mon_table[temp1])
           {
	     temp-=mon_table[temp1];//ƽ��
           }
           else
           { 
             break;
           }
         }                    
        temp1++; 
        }
                   
       timer.w_month=temp1+1;//�õ��·�
       timer.w_date=temp+1;  //�õ�����
     }
    temp=timecount%86400;     //�õ�������      
    timer.hour=temp/3600;     //Сʱ
    timer.min=(temp%3600)/60; //����     
    timer.sec=(temp%3600)%60; //����
    timer.w_week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);
    TimeDate_Display();
}
