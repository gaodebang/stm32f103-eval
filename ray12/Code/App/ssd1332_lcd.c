/********************************************************************************************************
* File Name          : ssd1332_lcd.c
*
* Description        : LCD module source file
*--------------------------------------------------------------------------------------------------------
* History            : 2012-2-7, xuanzg, File created
*
********************************************************************************************************/


#include "ssd1332_lcd.h"


#define COL_NUM 96
#define ROW_NUM 64


/// data cache for ssd1332 in SPI mode; read the pixel data
static  u16   PixelData[ROW_NUM][COL_NUM];

static void LcdSel(u8 sel)
{
    if (sel == LCD_CMD_MARK) 
    {
      GPIO_ResetBits(LCD_DC_Port, LCD_DC_Pin);                /* Select the command register (RS low)                     */
  	} 
  	else 
  	{
      GPIO_SetBits(LCD_DC_Port, LCD_DC_Pin);                  /* Select the data    register (RS high)                    */
  	}
}
/********************************************************************************************************
*                                            SEND COMMAND
*--------------------------------------------------------------------------------------------------------
* Description : This function sends a byte of command to ssd1332

* Arguments   : cmd
* Returns     : none
********************************************************************************************************/

void LCD_Send_CMD(u8 cmd)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);  /// wait until Tx buffer empty
    
    LcdSel(LCD_CMD_MARK);                                      /// D/C pin set to low
    GPIO_ResetBits(LCD_CS_Port, LCD_CS_Pin);                  /// CS pin set to low
    
    SPI_I2S_SendData(SPI2, cmd);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);    /// wait until send complete
    
    GPIO_SetBits(LCD_CS_Port, LCD_CS_Pin);                    /// CS pin set to high

}


/********************************************************************************************************
*                                            SEND DATA
*--------------------------------------------------------------------------------------------------------
* Description : This function sends a byte of data to ssd1332
* Arguments   : data
* Returns     : none
********************************************************************************************************/

void LCD_Send_DATA(u8 data)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);  /// wait until Tx buffer empty
    
    LcdSel(LCD_DATA_MARK);                                     /// D/C pin set to high
    GPIO_ResetBits(LCD_CS_Port, LCD_CS_Pin);                  /// CS pin set to low
    
    SPI_I2S_SendData(SPI2, data);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);    /// wait until send complete
    
    GPIO_SetBits(LCD_CS_Port, LCD_CS_Pin);                    /// CS pin set to high

}


/********************************************************************************************************
*                                            CLEAR THE SCREEN
*--------------------------------------------------------------------------------------------------------
* Description : This function clears the display
* Arguments   : none
* Returns     : none
********************************************************************************************************/

void LcdClrScr (void)
{
    
    /// RAY12 screen clear commands
    LCD_Send_CMD(0x25);               /// Clear Window command
    LCD_Send_CMD(0x00);               /// Column Address of Start
    LCD_Send_CMD(0x00);               /// Row Address of Start
    LCD_Send_CMD(0x5F);               /// Column Address of End
    LCD_Send_CMD(0x3F);               /// Row Address of End
       
    Delay_Ms(5);                  		/// Delay at least  2 mS
}


/********************************************************************************************************
*                                      DISPLAY DRIVER INITIALIZATION
*--------------------------------------------------------------------------------------------------------
* Description : This function initializes display
* Arguments   : none
* Returns     : none
********************************************************************************************************/

void LcdInit (void)
{

		//set dispay off
    LCD_Send_CMD(0xAE);
    
    //select external VCC pwr
    LCD_Send_CMD(0xAD); 
    LCD_Send_CMD(0x8E);
    
       
    //display remap;65K color;enable COM split odd even(hardware decide)
    LCD_Send_CMD(0xA0); 
    LCD_Send_CMD(0x72); 		//设置颜色位为16位，x、y方向的镜像
    
    //set normal dispay
    LCD_Send_CMD(0xA4);       
                    
      
    //set dispay on
    LCD_Send_CMD(0xAF);
    
    LcdClrScr();    
    
    for(u8 i = 0; i < 64;i++)
    {
        for(u8 j = 0; j < 96;j++)
        {
        	if(i%2==0)
        	{
        		if(j%2==0)
        		{
            	PixelData[i][j] = 0xffff;
          	}
          	else
          	{
          		PixelData[i][j] = 0;
          	}
          }
          else
         	{
         		if(j%2==1)
        		{
            	PixelData[i][j] = 0xffff;
          	}
          	else
          	{
         			PixelData[i][j] = 0;
         		}
         	}
         	//
         	PixelData[i][j] = 0;
         	LCD_SetPixel_RAY12(j,i,PixelData[i][j]) ;
        }
    }
}


/********************************************************************************************************
*                                               SET PIXEL
*--------------------------------------------------------------------------------------------------------
* Description : This function sets pixel
*               for ucgui calling; 16bit per pixel; x is coloum number; y is row number
* Arguments   : none
* Returns     : none
********************************************************************************************************/

void LCD_SetPixel_RAY12(u16 x, u16 y, u16 color)
{
    u16 HighBYTE;
    u16 LowBYTE;
    
    if(x > COL_NUM || y > ROW_NUM)
    {
        return;
    }

    HighBYTE = (u16)(color >> 8);
    LowBYTE = (u16)(color & 0x00FF);
  
    PixelData[y][x] = color; 

    
    LCD_Send_CMD(0x15);           /// set column address
    LCD_Send_CMD(x);              /// set column first address
    LCD_Send_CMD(x);              /// set column end address
    LCD_Send_CMD(0x75);           /// set row address
    LCD_Send_CMD(y);              /// set row first address
    LCD_Send_CMD(y);              /// set row end address

    LCD_Send_DATA(HighBYTE);
    LCD_Send_DATA(LowBYTE);
}


/********************************************************************************************************
*                                            GET PIXEL
*--------------------------------------------------------------------------------------------------------
* Description : This function gets pixel
*               for ucgui calling; 16bit per pixel; x is coloum number; y is row number
* Arguments   : none
* Returns     : none
********************************************************************************************************/

u16 LCD_GetPixel_RAY12(u16 x, u16 y)
{
    return PixelData[y][x];
}

void updata_lcd_display(void)
{
	static u8 i=0,j=0;
	if(++j>=96)
	{
		j=0;
		if(++i>=64)
		{
			i=0;
		}
	}
	LCD_SetPixel_RAY12(j,i,PixelData[i][j]) ;
}

void LCD_ON(void)
{
	    //set dispay on
    LCD_Send_CMD(0xAF);
}

void LCD_OFF(void)
{
	    //set dispay on
    LCD_Send_CMD(0xAE);
}