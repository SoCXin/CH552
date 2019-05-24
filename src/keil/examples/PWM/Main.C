
/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 PWM初始化，占空比设置，PWM默认电平设置
                       支持中断方式修改PWM占空比 
*******************************************************************************/

#include "..\Public\CH554.H"                                                   
#include "..\Public\Debug.H"
#include "PWM.H"
#include "stdio.h"

#pragma  NOAREGS

main( ) 
{
    CfgFsys( );                                                                //CH554时钟选择配置   
    mDelaymS(5);                                                               //配置时钟后，建议延时稳定时钟
    mInitSTDIO( );                                                             //串口0初始化
    printf("start ...\n"); 

    P1_MOD_OC &= ~(bPWM1 | bPWM2);                                             //设置PWM引脚为推挽输出
    P1_DIR_PU |= bPWM1 | bPWM2;			
    
    SetPWMClk(4);                                                              //PWM时钟配置	，4分频
    ForceClearPWMFIFO( );                                                      //强制清除PWM FIFO和COUNT
    CancleClearPWMFIFO( );                                                     //取消清除PWM FIFO和COUNT
    PWM1OutEnable( );                                                          //允许PWM1输出                           
    PWM2OutEnable( );                                                          //允许PWM2输出 	

    PWM1OutPolarHighAct( );                                                    //PWM1输出默认低，高有效                                                   
    PWM2OutPolarLowAct( );                                                     //PWM2输出默认高，低有效 

#if PWM_INTERRUPT
    PWMInterruptEnable();	
    EA = 1;
    SetPWM1Dat(0x10);                  
    SetPWM2Dat(0x40);	
    while(1);
#endif	
    SetPWM1Dat(0x10);                                                          //占空比0x10/256                                                         
    SetPWM2Dat(0x40);	
    while(1){
      if(PWM_CTRL&bPWM_IF_END){
        PWM_CTRL |= bPWM_IF_END;                                               //清除PWM中断				
        SetPWM1Dat(0x10);
        SetPWM2Dat(0x40);	
#ifdef DE_PRINTF
    printf("PWM_CYC_END  %02X\n",(UINT16)PWM_CTRL);
#endif	
      }
    }
}