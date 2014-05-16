#define		DISPLAY_GLOBALS
#include "display.h"

#define GUI_NOW_FONT GUI_FontHZ_SimSun_12

static uc8 wine_0[]						={" 洋河,梦之蓝,M3\n\r52度,500ml, 苏州"};
static uc8 wine_1[]						={" 洋河,天之蓝,M3\n\r52度,500ml, 苏州"};
static uc8 wine_2[]						={" 洋河,海之蓝,M3\n\r52度,500ml, 苏州"};

static uc8 * wine_addr[]      ={wine_0,wine_1,wine_2};


void DispInit(void)
{	
	LcdInit();
	LCD_OFF();
	GUI_Init();	
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
	GUI_DrawBitmap(&bm_yanghe_logo,0,0);
	LCD_ON();
	Delay_Ms(2000);
	GUI_SetFont(&GUI_NOW_FONT);
	Display_Welcome();
}

void Display_Welcome(void)
{	
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();	
	DisplayBattery();
	
  GUI_SetColor(GUI_CYAN);
  GUI_DispStringAt("欢迎使用",24,20);
  GUI_DispStringAt("UHF RFID读写器",6,40); 
}

void DisplayBattery(void)
{
    u16 ADC1_value; 
    u32 temp_color;
    ADC1_value = ADC_GetConversionValue(ADC1); 
    GUI_SetFont(&GUI_Font6x8);
    GUI_SetColor(GUI_WHITE);
    GUI_DispStringAt("RFID",3,0);
    temp_color=GUI_GetColor();
		GUI_SetColor(GUI_GREEN);
    if (ADC1_value > 2500)
    {
    	GUI_DrawPixel(80,3);
    	GUI_DrawPixel(80,4);
    	GUI_FillRect (81,0,95,7);
    }
    else if (ADC1_value > 2350)
    {
    	GUI_DrawPixel(80,3);
    	GUI_DrawPixel(80,4);
    	GUI_FillRect (81,0,95,7);
    	GUI_ClearRect(82,1,86,6);
    }
    else if(ADC1_value > 2300)
    {
    	GUI_DrawPixel(80,3);
    	GUI_DrawPixel(80,4);
    	GUI_FillRect (81,0,95,7);
    	GUI_ClearRect(82,1,90,6);
    }
    else
    {
    	GUI_SetColor(GUI_RED);
    	GUI_DrawPixel(80,3);
    	GUI_DrawPixel(80,4);
    	GUI_FillRect (81,0,95,7);
    	GUI_ClearRect(82,1,94,6);
    }
    GUI_SetColor(temp_color);
    GUI_SetFont(&GUI_NOW_FONT);
}

void Display_wait(u8 datain)
{
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	DisplayBattery();
	GUI_SetColor(GUI_CYAN);
	
	if(datain==0)
	{
  	GUI_DispStringAt("正在验证白酒",12,20);
  	GUI_DispStringAt("防伪信息 Wait...",0,40);	
	}
	else
	{
  	GUI_DispStringAt("正在生成在线",12,20);
		GUI_DispStringAt("识别信息 Wait...",0,40);
	}
}

void Display_Wine_Message(u8 * datain)
{
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	DisplayBattery();

	GUI_SetColor(GUI_BLUE);

	GUI_DispStringAt(wine_addr[(*datain)%3], 0, 20);
}

void Display_Sn_Data(u8 *datain)
{
	u8 i;
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	DisplayBattery();
	GUI_SetColor(GUI_RED);

	GUI_DispStringAt("SN:", 0, 20);
	GUI_GotoXY(12, 34);
	
	if(*datain>=45)
	{
		if((*(datain+1)&0xf0)!=0xf0)
		{
			*(datain+1)|=0xf0;
		}
	}
	else
	{
		if((*(datain+1)&0xf0)==0xf0)
		{
			*(datain+1)^=~0x40;
		}
	}
	
	for (i = 0; i < 6; i++)
	{
		GUI_DispHex(*(datain+i+1),2);
	}
}

void Display_User_Data(u8 *datain,u8 blank)
{
	u8 i;
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	DisplayBattery();
	GUI_SetColor(GUI_RED);
	GUI_GotoXY(0, 10);
	
  for (i = 0; i < 32; i++)
  {
      if ((i != 0) && ((i % 8) == 0))
      {
          GUI_DispChar('\n');
          GUI_DispHex(datain[i+blank*32],2);
      }
      else			
      {
          GUI_DispHex(datain[i+blank*32],2);
      }
  }
}
void Display_Serianet_Mark(void)
{
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();
	DisplayBattery();
	GUI_SetColor(GUI_MAGENTA);
	
	
	GUI_DispStringAt("串口透传模式", 12, 20);
	GUI_DispStringAt("Serianet", 24,040);
}


void Display_Sn_Qr(void)
{
	GUI_SetBkColor(GUI_WHITE);
  GUI_Clear();

	GUI_DrawBitmapMag (&bmQR,23,7,2,2);
}
