#ifndef RTC_H
#define RTC_H
#include "sys_config.h"
//时间结构体，包括年月日小时分秒
typedef struct
{
       u16 w_year;
       u8  w_month;
       u8  w_date;
       u8  w_week;
       u8  hour;
       u8  min;
       u8  sec;                             
}tm;

extern  u8 Time[8];  //存放时间

void RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
void RTC_Get(void);
void RTC_Set_Time(u8 *time);

#endif