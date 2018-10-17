
/********************************** (C) COPYRIGHT ******************************
* File Name          : CH554IAPDemo.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : 上电运行后P17LED灯闪烁，当检测“EnableIAP”脚为低电平后，将从用户程序跳转至BOOT，通过BOOT升级用户程序 
*******************************************************************************/
#include "./Public/CH554.H"                                                    
#include "./Public/Debug.H"

sbit EnableIAP  = P1^6;         
#define BOOT_ADDR  0x3800

#pragma NOAREGS

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
*                ：
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
typedef void( *pTaskFn)( void );

pTaskFn tasksArr[1]; 
 
void main( void ) 
{
	UINT16 i=0;
    while(1){
    SCK = ~SCK;                                                              //P17闪烁
    mDelaymS(50);
    if(EnableIAP == 0){                                                      //P16引脚检测到低电平跳转
      break;
    }
  }
  EA = 0;                                                                    //关闭总中断，必加
	tasksArr[0] = BOOT_ADDR;
  mDelaymS( 100 ); 				
  (tasksArr[0])( );                                                          //跳至BOOT升级程序,使用ISP工具升级	
  while(1); 
}
