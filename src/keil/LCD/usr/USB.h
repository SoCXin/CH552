/********************************** (C) COPYRIGHT *******************************
* File Name          : USB.C											*	
* Author             : QITAS                                                     *
* Version            : V1.0                                                     *
* Date               : 2018/06/06                                               *
* Description        : USB通信定义 
********************************************************************************/


#ifndef	__USB_H__
#define __USB_H__

#define		L_WIN 					0X08
#define 	L_ALT 					0X04
#define		L_SHIFT					0X02
#define 	L_CTL					0X01
#define 	R_WIN 					0X80
#define 	R_ALT 					0X40
#define 	R_SHIFT					0X20
#define 	R_CTL					0X10
#define 	SPACE					0X2C
#define		ENTER					0X28

#define		PowerON 				26 //&
#define 	PowerOFF 				21 //！
#define		KEY_1 					65 //A
#define 	KEY_2 					66
#define		KEY_3					67
#define 	KEY_4					68
#define 	KEY_5 					69
#define 	KEY_6 					70
#define 	KEY_0 					71

//发送通道选择，通过TXflag标记
#define 	SENDK					0x02
#define 	SENDD					0x04


//发送通道选择，通过RXflag标记
#define 	ERROR					0xEE
#define 	LONG					0xFF



//帧头数据
#define HEADER_MAGIC     0x5A

//上位机发送给MCU的命令类型

#define CMD_SET_USB  		0x01
#define CMD_SET_POP   		0x02
#define CMD_SET_TST   		0x40

//MCU返回给上位机的命令类型
#define RTNN                  0x60
#define CMD_RTN_SET_DISPLAY   RTNN+CMD_SET_DISPLAY
#define CMD_RTN_GET_VOLUME    RTNN+CMD_GET_VOLUME

extern UINT16I 	TouchKeyButton;	

//extern UINT8X 	TX[64];			//发送数据
//extern UINT8X 	RX[64];			//放置其他接收数据
extern UINT8X 	USB_BUF[128];

extern UINT8 	TXflag;
extern UINT8 	RXflag;

extern	void 	USBDeviceInit();
extern	void 	TXflagHandle(UINT8 val); //val参数根据TXflag有不同的意义
	
#endif
/**************************** END *************************************/
