#include <stdio.h>
#include <string.h>
#include <intrins.h> 


#include "CH554.h"
#include "common.h"
#include "Timer.h"
#include "USB.h"
#include "GPIO.h"

#include "lcd.h"
#include "font.h"
/******************************************************
GND=GND
VCC=3.3V
P00=CLK
P01=MOSI
P02=RES
P03=DC
P04=CS1
BLK为LCD背光控制管脚，低电平关闭背光，高电平打开；默认可以不接打开背光
////////////////////////以上是LCD工作所需要的/////////////////////////////////////
P05=FSO
P06=CS2

******************************************************/

u8 ref=0;//刷新显示

void xianshi()//显示信息
{   
	u16 lx,ly;
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;	
	showhanzi(10,0,0);  //中
	showhanzi(45,0,1);  //景
	showhanzi(80,0,2);  //园
    LCD_ShowString(10,35,"3.2 TFT SPI 240*320");
	LCD_ShowString(10,55,"LCD_W:");	LCD_ShowNum(70,55,LCD_W,3);
	LCD_ShowString(110,55,"LCD_H:");LCD_ShowNum(160,55,LCD_H,3);	
	lx=10;ly=75;			
}

void showimage() //显示40*40图片
{
  	int i,j,k; 
	//LCD_Clear(WHITE); //清屏  
	//xianshi(); //显示信息
	for(k=3;k<4;k++)
	{
	   	for(j=0;j<3;j++)
		{	
			Address_set(40*j,40*k,40*j+39,40*k+39);		//坐标设置
		    for(i=0;i<1600;i++)
			 { 				
			  	 //LCD_WR_DATA8(image[i*2+1]);	 
				 //LCD_WR_DATA8(image[i*2]);				
			 }	
		 }
	}
	ref=0;				
}
 
void main1()
{ 	
	//Set_All_GPIO_Quasi_Mode;					// Define in Function_define.h
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;
	//Lcd_Init();   		//tft初始化
	while(1)
	{
		LCD_CS1=0;
		LCD_DC=1;
		LCD_REST=0;
		delayms (200);	
		LCD_CS1=1;
		LCD_DC=0;
		LCD_REST=1;
		delayms (200);	
		
		//delayms (200);	
    }
}


 
void main()
{ 	
	//Set_All_GPIO_Quasi_Mode;					// Define in Function_define.h
	BACK_COLOR=WHITE;
	POINT_COLOR=RED;
	Lcd_Init();   		//tft初始化
	LCD_Clear(WHITE); 	//清屏
	while(1)
	{
		//Display_Asc_String('6',0,0, "2.4'TFT");	 //ASC 16X32点阵  
		//Display_Asc_String('1',0,34, "ASCII_5x7");//ASC 5X7点阵
		//Display_Asc_String('2',0,45, "ASCII_7x8");  //ASC 7X8点阵
		//Display_Asc_String('3',0,54, "ASCII_6x12");	//ASC 6X12点阵
		//Display_Asc_String('4',0,70, "ASCII_8x16");	//ASC 8X16点阵
		//Display_Asc_String('5',0,90, "AS12x24");//ASC 12X24点阵
		//Display_Asc_String('6',0,130, "16x32");	 //ASC 16X32点阵 
		delayms (2000);	
		LCD_Clear(RED); //清屏
		//Display_GB2312_String('1',0,0, "专业显示屏");//12x12汉字
		//Display_GB2312_String('2',0,16, "中景园电子");//15x16汉字
		//Display_GB2312_String('3',0,40, "中景园电子"); //24x24汉字
		//Display_GB2312_String('4',16,70, "中景园"); //32x32汉字
		//showimage(); //显示40*40图片
		delayms (2000);	
		LCD_Clear(BLUE); //清屏
    }
}




