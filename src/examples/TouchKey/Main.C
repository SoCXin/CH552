
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 触摸按键中断和查询方式进行采集并报告当前采样通道按键状态，包含初始化和按键采样等演示函数  
*******************************************************************************/
#include "..\Public\CH554.H"                                                   
#include "..\Public\Debug.H"
#include "TouchKey.H"
#include "stdio.h"

#pragma  NOAREGS

main( ) 
{
    UINT8 i;
    CfgFsys( );                                                                //CH554时钟选择配置   
    mDelaymS(5);                                                               //修改主频建议稍加延时等待芯片供电稳定	
    mInitSTDIO( );                                                             //串口0初始化
    printf("start ...\n"); 

	  P1_DIR_PU &= 0x0C;                                                         //所有触摸通道设置为浮空输入，用不到的通道可以不设置
    TouchKeyQueryCyl2ms();                                                     //TouchKey查询周期2ms	
    GetTouckKeyFreeBuf();                                                      //获取采样基准值
#if DE_PRINTF
    for(i=KEY_FIRST;i<(KEY_LAST+1);i++)                                        //打印采样基准值
    {
      printf("Channel %02x base sample %04x\n",(UINT16)i,KeyFree[i]);
		}
#endif

#if INTERRUPT_TouchKey
    EA = 1;
    while(1)                                                                   
    {
      if(KeyBuf)                                                               //key_buf非0，表示检测到按键按下
      {                       			
        printf("INT TouchKey Channel %02x \n",(UINT16)KeyBuf);                 //打印当前按键状态通道	
        KeyBuf	= 0;                                                           //清除按键按下标志   				
        mDelaymS(100);                                                         //延时无意义，模拟单片机做按键处理			
      }
      mDelaymS(100);                                                           //延时无意义，模拟单片机干其他事				
    } 
#else	
    while(1)                                                                   
    {			
      TouchKeyChannelQuery();                                                  //查询触摸按键状态
      if(KeyBuf)                                                               //key_buf非0，表示检测到按键按下
      {                        			
        printf("Query TouchKey Channel %02x \n",(UINT16)KeyBuf);               //打印当前按键状态通道				
        KeyBuf	= 0;                                                           //清除按键按下标志  				
        mDelaymS(100);                                                         //延时无意义，模拟单片机做按键处理			
      }
      mDelaymS(100);                                                           //延时无意义，模拟单片机干其他事				
    }
#endif
}