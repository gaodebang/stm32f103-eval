#include "rtc.h"

u8 Time[8] = {0};   //保存时间，包括年月日时分

tm timer;  //结构体，存放年月日时分秒


//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
static u8 Is_Leap_Year(u16 year)
{                    
		return (year%(year%100 ? 4:400) ? 0:1);
}     

//月份数据表                                                                       
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表  
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
void RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
    u16 t;
    u32 seccount=0;
	    
    if(syear<2000||syear>2099)
    {
      return ;//syear范围1970-2099，此处设置范围为2000-2099
    }
          
    for(t=1970;t<syear;t++) //把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))
        {
          seccount+=31622400;//闰年的秒钟数
        }
        else 
        {
          seccount+=31536000;                    //平年的秒钟数
        }
    }
    smon-=1;
    for(t=0;t<smon;t++)         //把前面月份的秒钟数相加
    {
        seccount+=(u32) mon_table[t]*86400;//月份秒钟数相加
       
        if(t==1 && Is_Leap_Year(syear))
        {
          seccount+=86400;					//闰年2月份增加一天的秒钟数
        }
    }
    
    seccount+=((u32)sday-(u32)1)*(u32)86400;	//把前面日期的秒钟数相加
    seccount+=(u32)hour*(u32)3600;				//小时秒钟数
    seccount+=(u32)min*(u32)60;      		//分钟秒钟数
    seccount+=(u32)sec;									//最后的秒钟加上去
                                                
    RTC_SetCounter(seccount);   		//将秒数送入寄存器
		//等待操作完成
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
  //开启BKP的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
  //后备域解锁
  PWR_BackupAccessCmd(ENABLE);	
	RTC_Set(syear, smon, sday, hour, min, sec);
  //关闭BKP的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, DISABLE);
  //后备域锁定
  PWR_BackupAccessCmd(DISABLE);
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号                                                                                                                                                    
static u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{    
       u16 temp2;
       u8 yearH,yearL;
      
       yearH=year/100;     yearL=year%100;
       // 如果为21世纪,年份数加100 
       if (yearH>19)yearL+=100;
       // 所过闰年数只算1900年之后的 
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

//得到当前的时间
void RTC_Get(void)
{
    static u32 daycnt=0;
    u32 timecount=0;
    u32 temp  = 0;
    u32 temp1 = 0;
 
    timecount = RTC_GetCounter();      
 
    temp=timecount/86400;   //得到天数(秒钟数对应的)
    if(daycnt!=temp)//超过一天了
    {      
       daycnt=temp;
       temp1=1970;  //从1970年开始
       while(temp>=365)
       {                         
         if(Is_Leap_Year(temp1))//是闰年
          {
             if(temp>=366)
             {
               temp-=366;//闰年的秒钟数
             }
             else
             {
               temp1++;
               break;
             } 
          }
         else 
         {
            temp-=365;       //平年
         }
          temp1++; 
        }  

        timer.w_year=temp1;//得到年份
        temp1=0;
        while(temp>=28)//超过了一个月
        {
          if(temp1==1 && Is_Leap_Year(timer.w_year))//当年是不是闰年/2月份
         {
           if(temp>=29)
            
             temp-=29;//闰年的秒钟数
  
           else
             break;
          }
         else
         {
           if(temp>=mon_table[temp1])
           {
	     temp-=mon_table[temp1];//平年
           }
           else
           { 
             break;
           }
         }                    
        temp1++; 
        }
                   
       timer.w_month=temp1+1;//得到月份
       timer.w_date=temp+1;  //得到日期
     }
    temp=timecount%86400;     //得到秒钟数      
    timer.hour=temp/3600;     //小时
    timer.min=(temp%3600)/60; //分钟     
    timer.sec=(temp%3600)%60; //秒钟
    timer.w_week=RTC_Get_Week(timer.w_year,timer.w_month,timer.w_date);
    TimeDate_Display();
}

