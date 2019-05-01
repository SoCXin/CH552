
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : GPIO 设置与使用和GPIO中断使用示例   
*******************************************************************************/
#include "..\Public\CH554.H"                                                   
#include "..\Public\Debug.H"
#include "GPIO.H"
#include "stdio.h"
#include <string.h>

#pragma  NOAREGS

void main( ) 
{
    UINT16 j = 0;
    CfgFsys( );                                                                //CH554时钟选择配置   
    mDelaymS(20);
    mInitSTDIO( );                                                             //串口0初始化
    printf("start ...\n"); 

#if GPIO_INTERRUPT
    GPIOInterruptCfg();                                                        //GPIO中断配置函数	
    EA = 1;
    printf("Run"); 
    while(1){
      printf("."); 
      mDelaymS(50);			
    }
#endif	
    Port1Cfg(3,4);                                                             //P14设置成类51双向模式         
    while(1){
      SCS = ~SCS;
      mDelaymS(50);			
    }
}