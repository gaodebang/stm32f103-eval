
#include"sys_config.h"


#define black_value 					0
#define white_value 					0xffff
#define blue_value 						0x1f
#define green_value 					0x7e0
#define red_value 						0xf800

#define cyan_value						0x7ff
#define magenta_value					0xf81f
#define yellow_value					0xffe0


#define LCD_CMD_MARK          0
#define LCD_DATA_MARK         1

#define LCD_PWR_Port 	GPIOC
#define LCD_PWR_Pin 	GPIO_Pin_7

#define LCD_RST_Port 	GPIOB
#define LCD_RST_Pin 	GPIO_Pin_6
#define LCD_DC_Port 	GPIOB
#define LCD_DC_Pin 		GPIO_Pin_11
#define LCD_CS_Port 	GPIOB
#define LCD_CS_Pin 		GPIO_Pin_12

#define LCD_SCK_Port 	GPIOB
#define LCD_SCK_Pin 	GPIO_Pin_13
#define LCD_MOSI_Port GPIOB
#define LCD_MOSI_Pin 	GPIO_Pin_15

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  LcdClrScr(void);
void  LcdInit(void);

void  LCD_Send_CMD(u8 cmd);
void  LCD_Send_DATA(u8 data);

void  LCD_SetPixel_RAY12(u16 x, u16 y, u16 color);
u16  LCD_GetPixel_RAY12(u16 x, u16 y);
void LCD_ON(void);
void LCD_OFF(void);

void updata_lcd_display(void);
