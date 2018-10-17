
/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 IO 设置接口函数和GPIO中断函数  
*******************************************************************************/

#include "CH554.H"                                                          
#include "common.h"
#include "GPIO.h"
#include "stdio.h"
#include "USB.h"

extern UINT8 Value_CH552[14];
UINT8 Power_y=0;

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : Port1Cfg()
* Description    : 端口1配置
* Input          : Mode  0 = 浮空输入，无上拉
                         1 = 推挽输入输出
                         2 = 开漏输入输出，无上拉
                         3 = 类51模式，开漏输入输出，有上拉，内部电路可以加速由低到高的电平爬升		
                   ,UINT8 Pin	(0-7)											 
* Output         : None
* Return         : None
*******************************************************************************/
void Port1Cfg(UINT8 Mode,UINT8 Pin)
{
  switch(Mode){
    case 0:
      P1_MOD_OC = P1_MOD_OC & ~(1<<Pin);
      P1_DIR_PU = P1_DIR_PU &	~(1<<Pin);	
      break;
    case 1:
      P1_MOD_OC = P1_MOD_OC & ~(1<<Pin);
      P1_DIR_PU = P1_DIR_PU |	(1<<Pin);				
      break;		
    case 2:
      P1_MOD_OC = P1_MOD_OC | (1<<Pin);
      P1_DIR_PU = P1_DIR_PU &	~(1<<Pin);				
      break;		
    case 3:
      P1_MOD_OC = P1_MOD_OC | (1<<Pin);
      P1_DIR_PU = P1_DIR_PU |	(1<<Pin);			
      break;
    default:
      break;			
  }
}

/*******************************************************************************
* Function Name  : Port3Cfg()
* Description    : 端口3配置
* Input          : Mode  0 = 浮空输入，无上拉
                         1 = 推挽输入输出
                         2 = 开漏输入输出，无上拉
                         3 = 类51模式，开漏输入输出，有上拉，内部电路可以加速由低到高的电平爬升		
                   ,UINT8 Pin	(0-7)											 
* Output         : None
* Return         : None
*******************************************************************************/
void Port3Cfg(UINT8 Mode,UINT8 Pin)
{
  switch(Mode){
    case 0:
      P3_MOD_OC = P3_MOD_OC & ~(1<<Pin);
      P3_DIR_PU = P3_DIR_PU &	~(1<<Pin);	
      break;
    case 1:
      P3_MOD_OC = P3_MOD_OC & ~(1<<Pin);
      P3_DIR_PU = P3_DIR_PU |	(1<<Pin);				
      break;		
    case 2:
      P3_MOD_OC = P3_MOD_OC | (1<<Pin);
      P3_DIR_PU = P3_DIR_PU &	~(1<<Pin);				
      break;		
    case 3:
      P3_MOD_OC = P3_MOD_OC | (1<<Pin);
      P3_DIR_PU = P3_DIR_PU |	(1<<Pin);			
      break;
    default:
      break;			
  }
}

/*******************************************************************************
* Function Name  : GPIOInterruptCfg()
* Description    : GPIO中断配置
* Input          : None									 
* Output         : None
* Return         : None
*******************************************************************************/
void GPIOInterruptCfg()
{
   GPIO_IE &= ~bIE_IO_EDGE;                                                    //高/低电平触发    低电平为有效电平
//   GPIO_IE |= bIE_IO_EDGE;                                                  //上升/下降触发    下降沿触发
   //GPIO_IE |= bIE_RXD1_LO;                                                     //RXD1低电平或下降沿触发  串口1接收中断使能
   GPIO_IE |= bIE_P1_5_LO | bIE_P1_4_LO | bIE_P1_3_LO | bIE_RST_HI;            
   //P15\P14\P13低电平触发；RST高电平触发  上升沿触发
   //GPIO_IE |= bIE_P3_1_LO;                                                     //P31低电平或下降沿触发
   //GPIO_IE |= bIE_RXD0_LO;                                                     //RXD0低电平或下降沿触发	串口2接收中断使能
   IE_GPIO  = 1;                                                               //GPIO中断开启   扩展中断使能
}


#ifdef GPIO_INTERRUPT
/*******************************************************************************
* Function Name  : GPIOInterrupt(void)
* Description    : GPIO 中断服务程序
*******************************************************************************/
void	GPIOInterrupt( void ) interrupt INT_NO_GPIO  using 1                      //GPIO中断服务程序,使用寄存器组1
{ 
//#ifdef DE_PRINTF
//      printf("GPIO_STATUS: %02x\n",(UINT16)(PIN_FUNC&bIO_INT_ACT));             
//#endif
}
#endif

/*******************************************************************************
* Function Name  : TheKeyQuery()
* Description    : 按键查询
* like-20180605
*******************************************************************************/
void TheKeyQuery()
{ 
	 if(KEY0==0)
	 {
		delayus(100);  
		if(KEY0==0)
		{
			TXflag=SENDK;			
			TXflagHandle(KEY_0);
		}						
	 }
	if(KEY1==0)
	{
		delayus(100);  
		if(KEY1==0)
		{
			TXflag=SENDK;			
      TXflagHandle(KEY_1);
		}						
	}
	if(KEY2==0)
	{
		delayus(100);   
		if(KEY2==0)
		{
			TXflag=SENDK;			
			TXflagHandle(KEY_2);
		}						
	}
	if(KEY3==0)
	{
		delayus(100);  
		if(KEY3==0)
		{
			TXflag=SENDK;			
      TXflagHandle(KEY_3);
		}						
	}
}
/*******************************************************************************
* Function Name  : TheKeyQuery()
* Description    : 电源查询
* like-20180605
*******************************************************************************/
void PowerSupplyQuery()
{ 
	 if(Power==0&&Power_y==1)
	 {
		TXflag=SENDK;			
		TXflagHandle(PowerOFF);
		Power_y = 0;
	 }
	 else if(Power==1&&Power_y==0)
	 {
		TXflag=SENDK;			
		TXflagHandle(PowerON);
		Power_y = 1;
	 }
}

