
/********************************** (C) COPYRIGHT *******************************
* File Name          : Touch_Key.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 触摸按键采样间隔设置、通道选择和切换和中断处理函数   
*******************************************************************************/

#include "CH554.H"                                                       
#include "DEBUG.H"
#include "Key.h"
#include "stdio.h"

#pragma  NOAREGS

/*******************************************************************************
* Function Name  : TouchKeyQueryCylSet1Or2ms(UINT8 cyl)
* Description    : 触摸按键查询周期设置
* Input          : UINT8 cyl 时钟设置                  								
                   1   2ms
                   0   1ms									 
* Output         : None
* Return         : None
*******************************************************************************/
void TouchKeyQueryCylSet1Or2ms(UINT8 cyl)
{
  if(cyl)
  {
    TKEY_CTRL |= bTKC_2MS;
  }
  else
  {
    TKEY_CTRL &= ~bTKC_2MS;
  }
}

/*******************************************************************************
* Function Name  : TouchKeyChanelSelect(UINT8 ch)
* Description    : 触摸按键通道选择
* Input          : UINT8 ch 采用通道
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL
*******************************************************************************/
UINT8 TouchKeyChanelSelect(UINT8 ch)
{
    TKEY_CTRL &= 0xF8;
    if(ch == 0){;}                                                            //关闭电容检测，仅作1ms或者2ms定时中断
    else if(ch == 1){P1_DIR_PU &= 0xFE; TKEY_CTRL |= 0x01;}                   //TIN0(P10)
    else if(ch == 2){P1_DIR_PU &= 0xFD; TKEY_CTRL |= 0x02;}                   //TIN0(P11)
    else if(ch == 3){P1_DIR_PU &= 0xEF; TKEY_CTRL |= 0x03;}                   //TIN0(P14)
    else if(ch == 4){P1_DIR_PU &= 0xDF; TKEY_CTRL |= 0x04;}                   //TIN0(P15)
    else if(ch == 5){P1_DIR_PU &= 0xBF; TKEY_CTRL |= 0x05;}                   //TIN0(P16)
    else if(ch == 6){P1_DIR_PU &= 0x7F; TKEY_CTRL |= 0x06;}                   //TIN0(P17)
    else if(ch == 7){TKEY_CTRL |= 0x07;}                                      //开启电容检测，但是不连接通道
    else return FAIL;
		
#if INTERRUPT_TouchKey
    IE_TKEY = 1;                                                               //使能Touch_Key中断
#endif
    return SUCCESS;
}

#if INTERRUPT_TouchKey
/*******************************************************************************
* Function Name  : Touch_KeyInterrupt(void)
* Description    : Touch_Key 中断服务程序
*******************************************************************************/
void	Touch_KeyInterrupt( void ) interrupt INT_NO_TKEY using 1                //Touch_Key中断服务程序,使用寄存器组1
{ 
    UINT16 KeyData;
    IE_TKEY = 0;                                                              //关中断	
    KeyData = TKEY_DAT;                                                       //保持87us,尽快取走
#ifdef DE_PRINTF
      printf("DATA: %04x\n",KeyData&0x7FFF);
#endif
}
#endif

