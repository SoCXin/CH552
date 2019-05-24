#ifndef __LCD_H
#define __LCD_H	

#include "CH554.h"
#include "common.h"
//#include "sys.h"

#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long
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

 //定义LCD的尺寸	
#define LCD_W 128
#define LCD_H 160

sbit LCD_CS1    =P1^4;    //片选	
sbit LCD_DC     =P1^3;	  //数据/命令切换
sbit LCD_SDI    =P1^1;	  //数据
sbit LCD_SCK    =P1^0;	  //时钟
//sbit LCD_REST   =P0^2;	  //复位 
sbit LCD_REST   =P1^5;	  //复位 

sbit ZK_MISO    =P1^7;	  //时钟
sbit ZK_CS2   =P1^6;	  //复位 

extern  u16 BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void Lcd_Init(void); 
void LCD_Clear(u16 Color);
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2);
void LCD_WR_DATA8(char da); //发送数据-8位参数
void LCD_WR_DATA(int da);
void LCD_WR_REG(char da);

void LCD_DrawPoint(u16 x,u16 y);//画点
void LCD_DrawPoint_big(u16 x,u16 y);//画一个大点
u16  LCD_ReadPoint(u16 x,u16 y); //读点
void Draw_Circle(u16 x0,u16 y0,u8 r);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode);//显示一个字符
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len);//显示数字
void LCD_Show2Num(u16 x,u16 y,u16 num,u8 len);//显示2个数字
void LCD_ShowString(u16 x,u16 y,const u8 *p);		 //显示一个字符串,16字体
 
void showhanzi(unsigned int x,unsigned int y,unsigned char index);

void CL_Mem(void);
void GBZK_ROMOUTSET(void);		//字库输出设置
void GBZK_ROMOUTRESET(void);	 	//字库输出设置
void GBZK_GPIO_Config(void);	 //字库芯片初始化
void ZK_command( unsigned char  dat );
unsigned char  get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(unsigned char  AddrHigh,unsigned char  AddrMid,unsigned char  AddrLow,unsigned char  *pBuff,unsigned char  DataLen );
void Display_Asc_String(unsigned char zk_num,unsigned int x, unsigned int y, unsigned char  text[]);
void Display_Asc( unsigned char zk_num,unsigned int x, unsigned int y );
void zk_map(  unsigned char *getdate, unsigned char *putdata, unsigned char high, unsigned char width);
void Display_GB2312_String(unsigned char zk_num,unsigned int  x,unsigned int  y, unsigned char  text[]);
void Display_GB2312( unsigned char zk_num,unsigned int x, unsigned int y );


//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


					  		 
#endif  
	 
	 



