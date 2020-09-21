
/********************************** (C) COPYRIGHT *******************************
* File Name          : MainSlave.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 SPI设备例子演示，连接SPI主机进行数据收发，主机获取设备的数据取反
                       然后发送给设备
*******************************************************************************/
#include "..\Public\CH554.H"                                                   
#include "..\Public\Debug.H"
#include "SPI.H"
#include "stdio.h"
#include "string.h"

/*硬件接口定义*/
/******************************************************************************
使用CH554 硬件SPI接口 
         CH554        DIR       
         P1.4        <==>       SCS
         P1.5        <==>       MOSI
         P1.6        <==>       MISO
         P1.7        <==>       SCK
*******************************************************************************/


void main()
{
    UINT8 ret,i=0;
    CfgFsys( ); 
    mDelaymS(5);                                                               //修改系统主频，建议稍加延时等待主频稳定    
    
    mInitSTDIO( );                                                             //串口0初始化
    printf("start ...\n");  
	
    SPISlvModeSet( );                                                          //SPI从机模式设置
// #if SPI0Interrupt
//     CH554SPIInterruptInit();                                                //SPI中断初始化
//     EA  = 1;                                                                //使能全局中断
// #endif
    while(1)
    {   
        CH554SPISlvWrite(i);                                                   //SPI等待主机把数据取走                                                  
        mDelayuS(2);                                                          
        ret = CH554SPISlvRead();                                               //读取SPI主机的数据
        if(ret != (i^0xff))
        {
		        printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //主机取反返回，如果值错误，打印错误数据					
        }
        mDelaymS(10);
        i = i+1;
        if((i%40)==0)                                                          //每成功40次打印一次
        {
            printf("success %02x\n",(UINT16)i);										
        }
    }
}