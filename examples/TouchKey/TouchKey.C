
/********************************** (C) COPYRIGHT *******************************
* File Name          : TouchKey.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 触摸按键采样间隔设置、通道选择和切换和中断处理函数   
*******************************************************************************/

#include "..\Public\CH554.H"                                                       
#include "..\Public\Debug.H"
#include "TouchKey.H"
#include "stdio.h"

#pragma  NOAREGS

UINT16	KeyFree[KEY_LAST-KEY_FIRST+1];                                        //触摸空闲值存储，用于比较按键状态，如果采样值小于基准值表明按键按下
UINT8V	KeyBuf;                                                               //触摸按键状态，为0表示无按键，非0表示当前检测按键被按下

/*******************************************************************************
* Function Name  : GetTouckKeyFreeBuf()
* Description    : 获取触摸按键空间状态键值
* Input          : None								 
* Output         : None
* Return         : None
*******************************************************************************/
void GetTouckKeyFreeBuf()
{
  UINT8 i,j;
  UINT8 TmpSum = 0;
  KeyBuf = 0;                                                                 //初始化设置为无按键状态
  for(i=KEY_FIRST;i<(KEY_LAST+1);i++)
  {
		j = KEY_BASE_SAMPLE_TIME;                                                 //采多次求平均值作为采样参考
	  TKEY_CTRL = (TKEY_CTRL & 0xF8 | i)+1;                                     //设置采样通道
    while(j--)
    {
        while((TKEY_CTRL&bTKC_IF) == 0);                                      //bTKC_IF变为1时，本周期采样完成
        TmpSum += TKEY_DAT&0x0F;                                              //采样值稳定，取低4位就够了
    }		
    KeyFree[i] = TKEY_DAT&0x07F0 + TmpSum/5;                                  //保存采样值 
  }
#if INTERRUPT_TouchKey
    IE_TKEY = 1;                                                              //使能Touch_Key中断
#endif   
}

/*******************************************************************************
* Function Name  : TouchKeyChannelSelect(UINT8 ch)
* Description    : 触摸按键通道选择
* Input          : UINT8 ch 采用通道
                   0~5 分别代表采样通道
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL  不支持的通道
*******************************************************************************/
UINT8 TouchKeyChannelSelect(UINT8 ch)
{
    if(ch < 6)
    {
        TKEY_CTRL = (TKEY_CTRL & 0xF8 | ch)+1;
        return SUCCESS;			
    }
    return FAIL;
}

#if INTERRUPT_TouchKey
/*******************************************************************************
* Function Name  : TouchKeyInterrupt(void)
* Description    : Touch_Key 中断服务程序
*******************************************************************************/
void	TouchKeyInterrupt( void ) interrupt INT_NO_TKEY using 1                //Touch_Key中断服务程序,使用寄存器组1
{ 
	  UINT8	ch;
    UINT16 KeyData;

    KeyData = TKEY_DAT;                                                       //保持87us,尽快取走
    ch = TKEY_CTRL&7;                                                         //获取当前采样通道
    if ( ch > KEY_LAST ){
       TKEY_CTRL = TKEY_CTRL & 0xF8 | KEY_FIRST;                              //从首通道开始采样
    }			
    else
    {
       TKEY_CTRL ++;                                                          //切换至下一个采样通道
    }
    if ( KeyData < (KeyFree[ch-KEY_FIRST] - KEY_ACT) )                        //如条件满足，代表按键按下   
    {
        KeyBuf=ch;                                                            //可以在此处进行按键动作处理或者置标志通知main进行处理
    }
}
#else
/*******************************************************************************
* Function Name  : TouchKeyChannelQuery()
* Description    : 触摸按键通道状态查询
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TouchKeyChannelQuery()
{
	  UINT8	ch;
    UINT16 KeyData;

    while((TKEY_CTRL&bTKC_IF) == 0);                                          //bTKC_IF变为1时，本周期采样完成
    KeyData = TKEY_DAT;                                                       //保持87us,尽快取走
    ch = TKEY_CTRL&7;                                                         //获取当前采样通道
    if ( ch > KEY_LAST ){
       TKEY_CTRL = TKEY_CTRL & 0xF8 | KEY_FIRST;                              //从首通道开始采样
    }			
    else
    {
       TKEY_CTRL ++;                                                          //切换至下一个采样通道
    }
    if ( KeyData < (KeyFree[ch-KEY_FIRST] - KEY_ACT) )                        //如条件满足，代表按键按下   
    {
        KeyBuf=ch;                                                            //可以在此处进行按键动作处理或者置标志通知main进行处理
    }
}
#endif

