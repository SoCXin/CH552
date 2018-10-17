
/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 ADC采样时钟设置，ADC通道设置函数，电压比较模式设置 
*******************************************************************************/

#include "..\Public\CH554.H"                                                          
#include "..\Public\Debug.H"
#include "ADC.H"
#include "stdio.h"

#pragma  NOAREGS

#define ADC_INTERRUPT  1

/*******************************************************************************
* Function Name  : ADCInit(UINT8 div)
* Description    : ADC采样时钟设置,模块开启，中断开启
* Input          : UINT8 div 时钟设置 
                   1 慢  384个Fosc                   								
                   0 快  96个Fosc									 
* Output         : None
* Return         : None
*******************************************************************************/
void ADCInit(UINT8 div)
{
    ADC_CFG &= ~bADC_CLK | div;
    ADC_CFG |= bADC_EN;                                                        //ADC电源使能
#if ADC_INTERRUPT
    ADC_IF = 0;                                                                //清空中断
    IE_ADC = 1;                                                                //使能ADC中断
#endif
}

/*******************************************************************************
* Function Name  : ADC_ChannelSelect(UINT8 ch)
* Description    : ADC采样启用
* Input          : UINT8 ch 采用通道
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL
*******************************************************************************/
UINT8 ADC_ChannelSelect(UINT8 ch)
{
    if(ch == 0){ADC_CHAN1 =0;ADC_CHAN0=0;P1_DIR_PU &= ~bAIN0;}                 //AIN0
    else if(ch == 1){ADC_CHAN1 =0;ADC_CHAN0=1;P1_DIR_PU &= ~bAIN1;}            //AIN1
    else if(ch == 2){ADC_CHAN1 =1;ADC_CHAN0=0;P1_DIR_PU &= ~bAIN2;}            //AIN2
    else if(ch == 3){ADC_CHAN1 =1;ADC_CHAN0=1;P3_DIR_PU &= ~bAIN3;}            //AIN3
    else return FAIL;
    return SUCCESS;
}

/*******************************************************************************
* Function Name  : VoltageCMPModeInit()
* Description    : 电压比较器模式初始化
* Input          : UINT8 fo 正向端口 0\1\2\3
                   UINT8 re 反向端口 1\3
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL
*******************************************************************************/
UINT8 VoltageCMPModeInit(UINT8 fo,UINT8 re)
{
    ADC_CFG |= bCMP_EN;                                                        //电平比较电源使能
    if(re == 1){
      if(fo == 0) {ADC_CHAN1 =0;ADC_CHAN0=0;CMP_CHAN =0;}                      //AIN0和AIN1
      else if(fo == 2) {ADC_CHAN1 =1;ADC_CHAN0=0;CMP_CHAN =0;}                 //AIN2和AIN1
      else if(fo == 3) {ADC_CHAN1 =1;ADC_CHAN0=1;CMP_CHAN =0; }                //AIN3和AIN1			
      else return FAIL;
    }			     
    else if(re == 3){
      if(fo == 0) {ADC_CHAN1 =0;ADC_CHAN0=0;CMP_CHAN =0;}                      //AIN0和AIN1
      else if(fo == 1) {ADC_CHAN1 =0;ADC_CHAN0=1;CMP_CHAN =0;}                 //AIN1和AIN1
      else if(fo == 2) {ADC_CHAN1 =1;ADC_CHAN0=0;CMP_CHAN =0;}                 //AIN2和AIN1			
      else return FAIL;
    }
    else return FAIL;
#if ADC_INTERRUPT
    CMP_IF = 0;                                                                //清空中断
    IE_ADC = 1;                                                                //使能ADC中断
#endif

     return SUCCESS;
}

#if ADC_INTERRUPT
/*******************************************************************************
* Function Name  : ADCInterrupt(void)
* Description    : ADC 中断服务程序
*******************************************************************************/
void	ADCInterrupt( void ) interrupt INT_NO_ADC using 1                       //ADC中断服务程序,使用寄存器组1
{ 
    if(ADC_IF ==  1)                                                          //ADC完成中断
    { 
//      UserData = ADC_DATA;                                                    //取走ADC采样数据
      ADC_IF = 0;		                                                          //清空ADC中断标志
    }
    if(CMP_IF ==  1)                                                          //电压比较完成中断
    {	
//       UserData = ADC_CTRL&0x80 >> 7);	                                        //保存比较器结果		
      CMP_IF = 0;		                                                          //清空比较器完成中断
    }
}
#endif

