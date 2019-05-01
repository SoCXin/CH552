
/********************************** (C) COPYRIGHT *******************************
* File Name          : VendorDefinedDev.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554模拟USB Module(CH554),厂商自定义接口设备，需要安装驱动，
驱动搜索CH37XDRV或者安装ISPTool会自动安装该设备类驱动，该设备类除了控制传输外，还是直插端点2批量上下传和端点1
中断上传，可以通过372DEBUG.EXE获取其他USB调试工具进行收发数据演示
            
*******************************************************************************/

#include "./Public/CH554.H"
#include "./Public/Debug.H"
#include <stdio.h>
#include <string.h>

#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE

// 设备描述符
UINT8C	MyDevDescr[] = { 0x12, 0x01, 0x10, 0x01,
						0xFF, 0x80, 0x55, THIS_ENDP0_SIZE,
						0x48, 0x43, 0x37, 0x55,  // 厂商ID和产品ID
						0x00, 0x01, 0x01, 0x02,
						0x00, 0x01 };
// 配置描述符
UINT8C	MyCfgDescr[] = { 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
						0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x55, 0x00,
						0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
						0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,
						0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x00 };
// 语言描述符
UINT8C	MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// 厂家信息
UINT8C	MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// 产品信息
UINT8C	MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '5', 0, '4', 0 };

UINT8	UsbConfig = 0;	// USB配置标志

UINT8X	Ep0Buffer[THIS_ENDP0_SIZE+2 >= MAX_PACKET_SIZE ? MAX_PACKET_SIZE : THIS_ENDP0_SIZE+2];  // OUT&IN
UINT8X	Ep1Buffer[MAX_PACKET_SIZE];                                                             // IN
UINT8X	Ep2Buffer[2*MAX_PACKET_SIZE];                                                           // OUT+IN

#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)

#pragma NOAREGS

void	USB_DeviceInterrupt( void ) interrupt INT_NO_USB using 1	      /* USB中断服务程序,使用寄存器组1 */
{
	UINT8	i, len;
	static	UINT8	SetupReqCode, SetupLen;
	static	PUINT8	pDescr;
	if ( UIF_TRANSFER ) {                                               // USB传输完成
		if ( U_IS_NAK ) {                                                 // not enable for this example
//			switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) ) {  // 分析操作令牌和端点号
//				case UIS_TOKEN_OUT | 2:                                     // endpoint 2# 批量端点下传
//					break;
//				case UIS_TOKEN_IN | 2:                                      // endpoint 2# 批量端点上传
//					break;
//				case UIS_TOKEN_IN | 1:                                      // endpoint 1# 中断端点上传
//					break;
//				default:
//					break;
//			}	
		}
		else {
			switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) ) {   // 分析操作令牌和端点号
				case UIS_TOKEN_OUT | 2:                                      // endpoint 2# 批量端点下传
					if ( U_TOG_OK ) {                                          // 不同步的数据包将丢弃
//						UEP2_CTRL ^= bUEP_R_TOG;                               // 已自动翻转
						len = USB_RX_LEN;
						for ( i = 0; i < len; i ++ ) {
							Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;    // OUT数据取反到IN由计算机验证
						}
						UEP2_T_LEN = len;
						UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;// 允许上传
					}
					break;
				case UIS_TOKEN_IN | 2:                                       // endpoint 2# 批量端点上传
//					UEP2_CTRL ^= bUEP_T_TOG;                                 // 已自动翻转
					UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;  // 暂停上传
					break;
				case UIS_TOKEN_IN | 1:                                       // endpoint 1# 中断端点上传
//					UEP1_CTRL ^= bUEP_T_TOG;                                 // 已自动翻转
					UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;  // 暂停上传
					break;
				case UIS_TOKEN_SETUP | 0:                                    // endpoint 0# SETUP
					len = USB_RX_LEN;
					if ( len == sizeof( USB_SETUP_REQ ) ) {                    // SETUP包长度
						SetupLen = UsbSetupBuf->wLengthL;
						if ( UsbSetupBuf->wLengthH || SetupLen > 0x7F ) SetupLen = 0x7F;// 限制总长度
						len = 0;                                                 // 默认为成功并且上传0长度
						SetupReqCode = UsbSetupBuf->bRequest;				
						if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD ) { /* 非标准请求 */
                len = 0xFF;					
						}
						else {                                                   // 标准请求
							switch( SetupReqCode ) {                               // 请求码
								case USB_GET_DESCRIPTOR:
									switch( UsbSetupBuf->wValueH ) {
										case 1:                                          // 设备描述符
											pDescr = (PUINT8)( &MyDevDescr[0] );
											len = sizeof( MyDevDescr );
											break;
										case 2:                                          // 配置描述符
											pDescr = (PUINT8)( &MyCfgDescr[0] );
											len = sizeof( MyCfgDescr );
											break;
										case 3:                                          // 字符串描述符
											switch( UsbSetupBuf->wValueL ) {
												case 1:
													pDescr = (PUINT8)( &MyManuInfo[0] );
													len = sizeof( MyManuInfo );
													break;
												case 2:
													pDescr = (PUINT8)( &MyProdInfo[0] );
													len = sizeof( MyProdInfo );
													break;
												case 0:
													pDescr = (PUINT8)( &MyLangDescr[0] );
													len = sizeof( MyLangDescr );
													break;
												default:
													len = 0xFF;                               // 不支持的字符串描述符
													break;
											}
											break;
										default:
											len = 0xFF;                                  // 不支持的描述符类型
											break;
									}
									if ( SetupLen > len ) SetupLen = len;            // 限制总长度
									len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // 本次传输长度
									memcpy( Ep0Buffer, pDescr, len );                 /* 加载上传数据 */
									SetupLen -= len;
									pDescr += len;
									break;
								case USB_SET_ADDRESS:
									SetupLen = UsbSetupBuf->wValueL;                  // 暂存USB设备地址
									break;
								case USB_GET_CONFIGURATION:
									Ep0Buffer[0] = UsbConfig;
									if ( SetupLen >= 1 ) len = 1;
									break;
								case USB_SET_CONFIGURATION:
									UsbConfig = UsbSetupBuf->wValueL;
									break;
								case USB_CLEAR_FEATURE:
									if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP ) {  // 端点
										switch( UsbSetupBuf->wIndexL ) {
											case 0x82:
												UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
												break;
											case 0x02:
												UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
												break;
											case 0x81:
												UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
												break;
											case 0x01:
												UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
												break;
											default:
												len = 0xFF;                                 // 不支持的端点
												break;
										}
									}
									else len = 0xFF;                                  // 不是端点不支持
									break;
								case USB_GET_INTERFACE:
									Ep0Buffer[0] = 0x00;
									if ( SetupLen >= 1 ) len = 1;
									break;
								case USB_GET_STATUS:
									Ep0Buffer[0] = 0x00;
									Ep0Buffer[1] = 0x00;
									if ( SetupLen >= 2 ) len = 2;
									else len = SetupLen;
									break;
								default:
									len = 0xFF;                                       // 操作失败
#ifdef DE_PRINTF								
									printf("ErrEp0ReqCode=%02X\n",(UINT16)SetupReqCode);
#endif								
									break;
							}
						}
					}
					else {
						len = 0xFF;                                            // SETUP包长度错误
#ifdef DE_PRINTF						
						printf("ErrEp0ReqSize\n");
#endif						
					}
					if ( len == 0xFF ) {                                     // 操作失败
						SetupReqCode = 0xFF;
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;  // STALL
					}
					else if ( len <= THIS_ENDP0_SIZE ) {                     // 上传数据或者状态阶段返回0长度包
						UEP0_T_LEN = len;
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // 默认数据包是DATA1
					}
					else {                                                   // 下传数据或其它
						UEP0_T_LEN = 0;  // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // 默认数据包是DATA1
					}
					break;
				case UIS_TOKEN_IN | 0:                                     // endpoint 0# IN
					switch( SetupReqCode ) {
						case USB_GET_DESCRIPTOR:
							len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // 本次传输长度
							memcpy( Ep0Buffer, pDescr, len );                    /* 加载上传数据 */
							SetupLen -= len;
							pDescr += len;
							UEP0_T_LEN = len;
							UEP0_CTRL ^= bUEP_T_TOG;                             // 翻转
							break;
						case USB_SET_ADDRESS:
							USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
							UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
							break;
						default:
							UEP0_T_LEN = 0;                                      // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
							UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
							break;
					}
					break;
				case UIS_TOKEN_OUT | 0:                                    // endpoint 0# OUT
					switch( SetupReqCode ) {
//						case download:
//							if ( U_TOG_OK ) {                                  // 不同步的数据包将丢弃
//								UEP0_CTRL ^= bUEP_R_TOG;                         // 翻转
//								                                                 //获取下传数据;
//								//UEP0_CTRL = UEP0_CTRL & bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // 预置上传0长度数据包DATA1以防主机提前进入状态阶段
//							}
//							break;
						case USB_GET_DESCRIPTOR:
						default:
							if ( U_TOG_OK ) {                                    // 不同步的数据包将丢弃
//								if ( USB_RX_LEN ) control_status_error;
//								else control_ok;                                 // 收到0长度包表示控制读操作/上传OK
							}
//							else control_status_error;
							UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;           // 准备下一控制传输
							break;
					}
					break;
				default:		
					break;
			}
		}
		UIF_TRANSFER = 0;                                              // 清中断标志
	}
	else if ( UIF_BUS_RST ) {                                        // USB总线复位
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
		UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
		USB_DEV_AD = 0x00;
		UIF_SUSPEND = 0;
		UIF_TRANSFER = 0;
		UIF_BUS_RST = 0;                                              // 清中断标志
	}
	else if ( UIF_SUSPEND ) {                                       // USB总线挂起/唤醒完成
		UIF_SUSPEND = 0;
		if ( USB_MIS_ST & bUMS_SUSPEND ) {                            // 挂起
#ifdef DE_PRINTF			
			printf( "zz" );                                             // 睡眠状态
#endif			
			while ( XBUS_AUX & bUART0_TX );                             // 等待发送完成
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                     // USB或者RXD0有信号时可被唤醒
			PCON |= PD;                                                 // 睡眠
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			WAKE_CTRL = 0x00;
		}
		else {                                                        // 唤醒
		}
	}
	else {                                                          // 意外的中断,不可能发生的情况
	
		USB_INT_FG = 0xFF;                                            // 清中断标志
	}
}

/*******************************************************************************
* Function Name  : InitUSB_Device()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void	InitUSB_Device( void )                                      // 初始化USB设备
{
	IE_USB = 0;
	USB_CTRL = 0x00;                                                // 先设定模式
	UEP4_1_MOD = bUEP1_TX_EN;                                       // 端点1上传IN
	UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;                         // 端点2下传OUT和上传IN
	UEP0_DMA = Ep0Buffer;
	UEP1_DMA = Ep1Buffer;
	UEP2_DMA = Ep2Buffer;
	UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
	UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
	UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
	USB_DEV_AD = 0x00;
	UDEV_CTRL = bUD_PD_DIS;                                         // 禁止DP/DM下拉电阻
	USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;           // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
	UDEV_CTRL |= bUD_PORT_EN;                                       // 允许USB端口
	USB_INT_FG = 0xFF;                                              // 清中断标志
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
	IE_USB = 1;
}       

main( ) {
	UINT8	i;
  CfgFsys();
  mDelaymS(5);                                                   //修改主频，稍加延时等待主频稳定	
	mInitSTDIO( );                                                 /* 初始化串口0为了让计算机通过串口监控演示过程 */
#ifdef DE_PRINTF	
	printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
#endif	
	InitUSB_Device( );
	EA = 1;
	while ( 1 ) {
		i = getkey( );
		printf( "%c", (UINT8)i );
		if ( i >= '0' && i <= 'z' ) {
			memcpy( Ep1Buffer, (PUINT8C)(i-'0'), MAX_PACKET_SIZE );     /* 加载上传数据 */
			UEP1_T_LEN = i-'0' > 8 ? 8 : i-'0';
			UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
		}  
	}
}
