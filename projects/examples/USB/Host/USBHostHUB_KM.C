
/********************************** (C) COPYRIGHT *******************************
* File Name          : USBHostHUB_KM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        :
 USB host example for CH554, start USB device under DP/DM and HP/HM port
 USB主机应用例子,初始化和枚举USB端口连接的设备,同时支持最多1个USB设备,支持一级外部HUB,
 可以操作USB键鼠和HUB,包含HID类命令处理
 如果需要操作U盘，请参考U_DISK文件夹下的例子
 支持简单USB打印机操作，没有处理USB打印机类命令 
*******************************************************************************/

#include "..\..\Public\CH554.H"                                               
#include "..\..\Public\Debug.H"
#include "..\USB_LIB\USBHOST.H"
#include <stdio.h>
#include <string.h>

#pragma  NOAREGS

/*获取设备描述符*/
UINT8C  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };

/*获取配置描述符*/
UINT8C  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };

/*设置USB地址*/
UINT8C  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*设置USB配置*/
UINT8C  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*设置USB接口配置*/
UINT8C  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*清除端点STALL*/
UINT8C  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*获取HUB描述符*/
UINT8C  SetupGetHubDescr[] = { HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof( USB_HUB_DESCR ), 0x00 };

/*获取HID设备报表描述符*/
UINT8C  SetupGetHIDDevReport[] = { 0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0x41, 0x00 };

/*打印机类命令*/
UINT8C  XPrinterReport[] = { 0xA1, 0, 0x00, 0, 0x00, 0x00, 0xF1, 0x03 };

UINT8X  UsbDevEndp0Size;                                                       //* USB设备的端点0的最大包尺寸 */
UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;                              // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0040 ;                              // OUT, must even address

UINT8 Set_Port = 0;

_RootHubDev xdata ThisUsbDev;                                                   //ROOT口
_DevOnHubPort xdata DevOnHubPort[HUB_MAX_PORTS];                                // 假定:不超过1个外部HUB,每个外部HUB不超过HUB_MAX_PORTS个端口(多了不管)

bit     RootHubId;                                                              // 当前正在操作的root-hub端口号:0=HUB0,1=HUB1
bit     FoundNewDev;

main( )
{
    UINT8   i, s, len, endp;
    UINT16  loc;
    CfgFsys( );	
    mDelaymS(5);	                                                              //修改主频，等待时钟稳定
    mInitSTDIO( );                                                              //为了让计算机通过串口监控演示过程
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
    InitUSB_Host( );
    FoundNewDev = 0;
    printf( "Wait Device In\n" );
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( UIF_DETECT ){                                                       // 如果有USB主机检测中断则处理
            UIF_DETECT = 0;                                                      // 清中断标志
            s = AnalyzeRootHub( );                                               // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT ) FoundNewDev = 1;						
        }
        if ( FoundNewDev ){                                                      // 有新的USB设备插入
            FoundNewDev = 0;
            mDelaymS( 200 );                                                     // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
            s = EnumAllRootDevice( );                                            // 枚举所有ROOT-HUB端口的USB设备
            if ( s != ERR_SUCCESS ){
                printf( "EnumAllRootDev err = %02X\n", (UINT16)s );
            }
        }
        if ( RI == 0 ) continue;
        i = getkey( );
        printf( "%c", (UINT8)i );
        if ( i == 'E'){                                                           // 每隔一段时间,通常是100mS到1000mS,对外部HUB的端口进行一下枚举,单片机有空时做
            s = EnumAllHubPort( );                                                // 枚举所有ROOT-HUB端口下外部HUB后的二级USB设备
            if ( s != ERR_SUCCESS ){                                              // 可能是HUB断开了
                printf( "EnumAllHubPort err = %02X\n", (UINT16)s );
            }
        }
        switch( i ){                                                              // 模拟主观请求,对某USB设备进行操作
        case 'M':                                                                 // 用定时模拟主观需求,需要操作鼠标
            loc = SearchTypeDevice( DEV_TYPE_MOUSE );                             // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
            if ( loc != 0xFFFF ){                                                 // 找到了,如果有两个MOUSE如何处理?
                printf( "Query Mouse @%04X\n", loc );
                i = (UINT8)( loc >> 8 );
                len = (UINT8)loc;
                SelectHubPort( len );                                             // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
                endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;        // 中断端点的地址,位7用于同步标志位
                if ( endp & USB_ENDP_ADDR_MASK ){                                 // 端点有效
                    s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554传输事务,获取数据,NAK不重试
                    if ( s == ERR_SUCCESS ){
                        endp ^= 0x80;                                             // 同步标志翻转
                        if ( len ) DevOnHubPort[len-1].GpVar = endp;              // 保存同步标志位
                        else ThisUsbDev.GpVar = endp;
                        len = USB_RX_LEN;                                         // 接收到的数据长度
                        if ( len ) {
                            printf("Mouse data: ");
                            for ( i = 0; i < len; i ++ ){
                                printf("x%02X ",(UINT16)(RxBuffer[i]) );
                            }
                            printf("\n");
                        }
                    }
                    else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ) {
                        printf("Mouse error %02x\n",(UINT16)s);                   // 可能是断开了
                    }
                }
                else {
                    printf("Mouse no interrupt endpoint\n");
                }
                SetUsbSpeed( 1 );                                                 // 默认为全速
            }
            break;
        case 'K':                                                                 // 用定时模拟主观需求,需要操作键盘
            loc = SearchTypeDevice( DEV_TYPE_KEYBOARD );                          // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
            if ( loc != 0xFFFF ){                                                 // 找到了,如果有两个KeyBoard如何处理?
                printf( "Query Keyboard @%04X\n", loc );
                i = (UINT8)( loc >> 8 );
                len = (UINT8)loc;
                SelectHubPort( len );                                             // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
                endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;        // 中断端点的地址,位7用于同步标志位
                printf("%02X  ",endp);
                if ( endp & USB_ENDP_ADDR_MASK ){                                 // 端点有效
                    s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554传输事务,获取数据,NAK不重试
                    if ( s == ERR_SUCCESS ){
                        endp ^= 0x80;                                             // 同步标志翻转
                        if ( len ) DevOnHubPort[len-1].GpVar = endp;              // 保存同步标志位
                        else ThisUsbDev.GpVar = endp;
                        len = USB_RX_LEN;                                         // 接收到的数据长度
                        if ( len ){
                            SETorOFFNumLock(RxBuffer);
                            printf("keyboard data: ");
                            for ( i = 0; i < len; i ++ ){
                                printf("x%02X ",(UINT16)(RxBuffer[i]) );
                            }
                            printf("\n");
                        }
                    }
                    else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ){
                        printf("keyboard error %02x\n",(UINT16)s);               // 可能是断开了
                    }
                }
                else{
                    printf("keyboard no interrupt endpoint\n");
                }
                SetUsbSpeed( 1 );                                                // 默认为全速
            }
            break;
        case 'H':                                                                 //操作HUB
            loc = SearchTypeDevice( DEV_TYPE_KEYBOARD );                          // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
            if ( loc != 0xFFFF ){                                                 // 找到了,如果有两个KeyBoard如何处理?
                printf( "Query Keyboard @%04X\n", loc );
                i = (UINT8)( loc >> 8 );
                len = (UINT8)loc;
                SelectHubPort( len );                                             // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
                endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;        // 中断端点的地址,位7用于同步标志位
                printf("%02X  ",endp);
                if ( endp & USB_ENDP_ADDR_MASK ){                                 // 端点有效
                    s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554传输事务,获取数据,NAK不重试
                    if ( s == ERR_SUCCESS ){
                        endp ^= 0x80;                                             // 同步标志翻转
                        if ( len ) DevOnHubPort[len-1].GpVar = endp;              // 保存同步标志位
                        else ThisUsbDev.GpVar = endp;
                        len = USB_RX_LEN;                                         // 接收到的数据长度
                        if ( len ) {
                            //SETorOFFNumLock(RxBuffer);
                            printf("keyboard data: ");
                            for ( i = 0; i < len; i ++ ){
                                printf("x%02X ",(UINT16)(RxBuffer[i]) );
                            }
                            printf("\n");
                        }
                    }
                    else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ){
                        printf("keyboard error %02x\n",(UINT16)s);                 // 可能是断开了
                    }
                }
                else printf("keyboard no interrupt endpoint\n");
            }
            for(i=0; i<2; i++) {
                if((ThisUsbDev.DeviceStatus == ROOT_DEV_SUCCESS)&&(ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB)){
                    SelectHubPort( 0 );                                            // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
                    endp = ThisUsbDev.GpVar1;                                      // 中断端点的地址,位7用于同步标志位
                    if ( endp & USB_ENDP_ADDR_MASK ){                              // 端点有效
                        s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554传输事务,获取数据,NAK不重试
                        if ( s == ERR_SUCCESS ) {
                            endp ^= 0x80;                                          // 同步标志翻转
                            ThisUsbDev.GpVar1 = endp;                              // 保存同步标志位
                            len = USB_RX_LEN;                                      // 接收到的数据长度
                            if ( len ){
                                EnumHubPort();
                                for ( i = 0; i < len; i ++ ){
                                    printf("x%02X ",(UINT16)(RxBuffer[i]) );
                                }
                                printf("\n");
                            }
                        }
                        else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ) {
                            printf("HUB error %02x\n",(UINT16)s);                    // 可能是断开了
                        }
                    }
                    else printf("HUB %02d no interrupt endpoint\n",i);
                }
                else printf("ROOTHUB %02d not HUB\n",i);
            }
            break;
        case 'P':                                                                   //操作USB打印机
					  if(TIN0 == 0){                                                          //P10为低，开始打印
							memset(TxBuffer,0,sizeof(TxBuffer));
              TxBuffer[0]=0x1B;TxBuffer[1]=0x40;TxBuffer[2]=0x1D;TxBuffer[3]=0x55;TxBuffer[4]=0x42;TxBuffer[5]=0x02;TxBuffer[6]=0x18;TxBuffer[7]=0x1D;
              TxBuffer[8]=0x76;TxBuffer[9]=0x30;TxBuffer[10]=0x00;TxBuffer[11]=0x30;TxBuffer[12]=0x00;TxBuffer[13]=0x18;
							loc = SearchTypeDevice( USB_DEV_CLASS_PRINTER );                       // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
							if ( loc != 0xFFFF ){                                                  // 找到了,如果有两个打印机如何处理?
									printf( "Query Printer @%04X\n", loc );
									i = (UINT8)( loc >> 8 );
									len = (UINT8)loc;
									SelectHubPort( len );                                              // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
									endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;         // 端点的地址,位7用于同步标志位
									printf("%02X  ",endp);  
									if ( endp & USB_ENDP_ADDR_MASK ){                                  // 端点有效
                      UH_TX_LEN = 64;                                                // 默认无数据故状态阶段为IN										
											s = USBHostTransact( USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff );// CH554传输事务,获取数据,NAK重试
											if ( s == ERR_SUCCESS ){
													endp ^= 0x80;                                               // 同步标志翻转
                          memset(TxBuffer,0,sizeof(TxBuffer));
                          UH_TX_LEN = 64;                                             // 默认无数据故状态阶段为IN										
											    s = USBHostTransact( USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff );// CH554传输事务,获取数据,NAK重试   											
											}
											else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ) printf("Printer error %02x\n",(UINT16)s); // 可能是断开了
											}
									}
							}				
            break;
        default:
            break;
        }
    }
}
