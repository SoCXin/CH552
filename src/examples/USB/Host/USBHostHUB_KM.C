
/********************************** (C) COPYRIGHT *******************************
* File Name          : USBHostHUB_KM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        :
 USB host example for CH554, start USB device under DP/DM and HP/HM port
 USB����Ӧ������,��ʼ����ö��USB�˿����ӵ��豸,ͬʱ֧�����1��USB�豸,֧��һ���ⲿHUB,
 ���Բ���USB�����HUB,����HID�������
 �����Ҫ����U�̣���ο�U_DISK�ļ����µ�����
 ֧�ּ�USB��ӡ��������û�д���USB��ӡ�������� 
*******************************************************************************/

#include "..\..\Public\CH554.H"                                               
#include "..\..\Public\Debug.H"
#include "..\USB_LIB\USBHOST.H"
#include <stdio.h>
#include <string.h>

#pragma  NOAREGS

/*��ȡ�豸������*/
UINT8C  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };

/*��ȡ����������*/
UINT8C  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };

/*����USB��ַ*/
UINT8C  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*����USB����*/
UINT8C  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*����USB�ӿ�����*/
UINT8C  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*����˵�STALL*/
UINT8C  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/*��ȡHUB������*/
UINT8C  SetupGetHubDescr[] = { HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof( USB_HUB_DESCR ), 0x00 };

/*��ȡHID�豸����������*/
UINT8C  SetupGetHIDDevReport[] = { 0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0x41, 0x00 };

/*��ӡ��������*/
UINT8C  XPrinterReport[] = { 0xA1, 0, 0x00, 0, 0x00, 0x00, 0xF1, 0x03 };

UINT8X  UsbDevEndp0Size;                                                       //* USB�豸�Ķ˵�0�������ߴ� */
UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;                              // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0040 ;                              // OUT, must even address

UINT8 Set_Port = 0;

_RootHubDev xdata ThisUsbDev;                                                   //ROOT��
_DevOnHubPort xdata DevOnHubPort[HUB_MAX_PORTS];                                // �ٶ�:������1���ⲿHUB,ÿ���ⲿHUB������HUB_MAX_PORTS���˿�(���˲���)

bit     RootHubId;                                                              // ��ǰ���ڲ�����root-hub�˿ں�:0=HUB0,1=HUB1
bit     FoundNewDev;

main( )
{
    UINT8   i, s, len, endp;
    UINT16  loc;
    CfgFsys( );	
    mDelaymS(5);	                                                              //�޸���Ƶ���ȴ�ʱ���ȶ�
    mInitSTDIO( );                                                              //Ϊ���ü����ͨ�����ڼ����ʾ����
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
    InitUSB_Host( );
    FoundNewDev = 0;
    printf( "Wait Device In\n" );
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( UIF_DETECT ){                                                       // �����USB��������ж�����
            UIF_DETECT = 0;                                                      // ���жϱ�־
            s = AnalyzeRootHub( );                                               // ����ROOT-HUB״̬
            if ( s == ERR_USB_CONNECT ) FoundNewDev = 1;						
        }
        if ( FoundNewDev ){                                                      // ���µ�USB�豸����
            FoundNewDev = 0;
            mDelaymS( 200 );                                                     // ����USB�豸�ղ�����δ�ȶ�,�ʵȴ�USB�豸���ٺ���,������ζ���
            s = EnumAllRootDevice( );                                            // ö������ROOT-HUB�˿ڵ�USB�豸
            if ( s != ERR_SUCCESS ){
                printf( "EnumAllRootDev err = %02X\n", (UINT16)s );
            }
        }
        if ( RI == 0 ) continue;
        i = getkey( );
        printf( "%c", (UINT8)i );
        if ( i == 'E'){                                                           // ÿ��һ��ʱ��,ͨ����100mS��1000mS,���ⲿHUB�Ķ˿ڽ���һ��ö��,��Ƭ���п�ʱ��
            s = EnumAllHubPort( );                                                // ö������ROOT-HUB�˿����ⲿHUB��Ķ���USB�豸
            if ( s != ERR_SUCCESS ){                                              // ������HUB�Ͽ���
                printf( "EnumAllHubPort err = %02X\n", (UINT16)s );
            }
        }
        switch( i ){                                                              // ģ����������,��ĳUSB�豸���в���
        case 'M':                                                                 // �ö�ʱģ����������,��Ҫ�������
            loc = SearchTypeDevice( DEV_TYPE_MOUSE );                             // ��ROOT-HUB�Լ��ⲿHUB���˿�������ָ�����͵��豸���ڵĶ˿ں�
            if ( loc != 0xFFFF ){                                                 // �ҵ���,���������MOUSE��δ���?
                printf( "Query Mouse @%04X\n", loc );
                i = (UINT8)( loc >> 8 );
                len = (UINT8)loc;
                SelectHubPort( len );                                             // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
                endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;        // �ж϶˵�ĵ�ַ,λ7����ͬ����־λ
                if ( endp & USB_ENDP_ADDR_MASK ){                                 // �˵���Ч
                    s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554��������,��ȡ����,NAK������
                    if ( s == ERR_SUCCESS ){
                        endp ^= 0x80;                                             // ͬ����־��ת
                        if ( len ) DevOnHubPort[len-1].GpVar = endp;              // ����ͬ����־λ
                        else ThisUsbDev.GpVar = endp;
                        len = USB_RX_LEN;                                         // ���յ������ݳ���
                        if ( len ) {
                            printf("Mouse data: ");
                            for ( i = 0; i < len; i ++ ){
                                printf("x%02X ",(UINT16)(RxBuffer[i]) );
                            }
                            printf("\n");
                        }
                    }
                    else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ) {
                        printf("Mouse error %02x\n",(UINT16)s);                   // �����ǶϿ���
                    }
                }
                else {
                    printf("Mouse no interrupt endpoint\n");
                }
                SetUsbSpeed( 1 );                                                 // Ĭ��Ϊȫ��
            }
            break;
        case 'K':                                                                 // �ö�ʱģ����������,��Ҫ��������
            loc = SearchTypeDevice( DEV_TYPE_KEYBOARD );                          // ��ROOT-HUB�Լ��ⲿHUB���˿�������ָ�����͵��豸���ڵĶ˿ں�
            if ( loc != 0xFFFF ){                                                 // �ҵ���,���������KeyBoard��δ���?
                printf( "Query Keyboard @%04X\n", loc );
                i = (UINT8)( loc >> 8 );
                len = (UINT8)loc;
                SelectHubPort( len );                                             // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
                endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;        // �ж϶˵�ĵ�ַ,λ7����ͬ����־λ
                printf("%02X  ",endp);
                if ( endp & USB_ENDP_ADDR_MASK ){                                 // �˵���Ч
                    s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554��������,��ȡ����,NAK������
                    if ( s == ERR_SUCCESS ){
                        endp ^= 0x80;                                             // ͬ����־��ת
                        if ( len ) DevOnHubPort[len-1].GpVar = endp;              // ����ͬ����־λ
                        else ThisUsbDev.GpVar = endp;
                        len = USB_RX_LEN;                                         // ���յ������ݳ���
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
                        printf("keyboard error %02x\n",(UINT16)s);               // �����ǶϿ���
                    }
                }
                else{
                    printf("keyboard no interrupt endpoint\n");
                }
                SetUsbSpeed( 1 );                                                // Ĭ��Ϊȫ��
            }
            break;
        case 'H':                                                                 //����HUB
            loc = SearchTypeDevice( DEV_TYPE_KEYBOARD );                          // ��ROOT-HUB�Լ��ⲿHUB���˿�������ָ�����͵��豸���ڵĶ˿ں�
            if ( loc != 0xFFFF ){                                                 // �ҵ���,���������KeyBoard��δ���?
                printf( "Query Keyboard @%04X\n", loc );
                i = (UINT8)( loc >> 8 );
                len = (UINT8)loc;
                SelectHubPort( len );                                             // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
                endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;        // �ж϶˵�ĵ�ַ,λ7����ͬ����־λ
                printf("%02X  ",endp);
                if ( endp & USB_ENDP_ADDR_MASK ){                                 // �˵���Ч
                    s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554��������,��ȡ����,NAK������
                    if ( s == ERR_SUCCESS ){
                        endp ^= 0x80;                                             // ͬ����־��ת
                        if ( len ) DevOnHubPort[len-1].GpVar = endp;              // ����ͬ����־λ
                        else ThisUsbDev.GpVar = endp;
                        len = USB_RX_LEN;                                         // ���յ������ݳ���
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
                        printf("keyboard error %02x\n",(UINT16)s);                 // �����ǶϿ���
                    }
                }
                else printf("keyboard no interrupt endpoint\n");
            }
            for(i=0; i<2; i++) {
                if((ThisUsbDev.DeviceStatus == ROOT_DEV_SUCCESS)&&(ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB)){
                    SelectHubPort( 0 );                                            // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
                    endp = ThisUsbDev.GpVar1;                                      // �ж϶˵�ĵ�ַ,λ7����ͬ����־λ
                    if ( endp & USB_ENDP_ADDR_MASK ){                              // �˵���Ч
                        s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH554��������,��ȡ����,NAK������
                        if ( s == ERR_SUCCESS ) {
                            endp ^= 0x80;                                          // ͬ����־��ת
                            ThisUsbDev.GpVar1 = endp;                              // ����ͬ����־λ
                            len = USB_RX_LEN;                                      // ���յ������ݳ���
                            if ( len ){
                                EnumHubPort();
                                for ( i = 0; i < len; i ++ ){
                                    printf("x%02X ",(UINT16)(RxBuffer[i]) );
                                }
                                printf("\n");
                            }
                        }
                        else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ) {
                            printf("HUB error %02x\n",(UINT16)s);                    // �����ǶϿ���
                        }
                    }
                    else printf("HUB %02d no interrupt endpoint\n",i);
                }
                else printf("ROOTHUB %02d not HUB\n",i);
            }
            break;
        case 'P':                                                                   //����USB��ӡ��
					  if(TIN0 == 0){                                                          //P10Ϊ�ͣ���ʼ��ӡ
							memset(TxBuffer,0,sizeof(TxBuffer));
              TxBuffer[0]=0x1B;TxBuffer[1]=0x40;TxBuffer[2]=0x1D;TxBuffer[3]=0x55;TxBuffer[4]=0x42;TxBuffer[5]=0x02;TxBuffer[6]=0x18;TxBuffer[7]=0x1D;
              TxBuffer[8]=0x76;TxBuffer[9]=0x30;TxBuffer[10]=0x00;TxBuffer[11]=0x30;TxBuffer[12]=0x00;TxBuffer[13]=0x18;
							loc = SearchTypeDevice( USB_DEV_CLASS_PRINTER );                       // ��ROOT-HUB�Լ��ⲿHUB���˿�������ָ�����͵��豸���ڵĶ˿ں�
							if ( loc != 0xFFFF ){                                                  // �ҵ���,�����������ӡ����δ���?
									printf( "Query Printer @%04X\n", loc );
									i = (UINT8)( loc >> 8 );
									len = (UINT8)loc;
									SelectHubPort( len );                                              // ѡ�����ָ����ROOT-HUB�˿�,���õ�ǰUSB�ٶ��Լ��������豸��USB��ַ
									endp = len ? DevOnHubPort[len-1].GpVar : ThisUsbDev.GpVar;         // �˵�ĵ�ַ,λ7����ͬ����־λ
									printf("%02X  ",endp);  
									if ( endp & USB_ENDP_ADDR_MASK ){                                  // �˵���Ч
                      UH_TX_LEN = 64;                                                // Ĭ�������ݹ�״̬�׶�ΪIN										
											s = USBHostTransact( USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff );// CH554��������,��ȡ����,NAK����
											if ( s == ERR_SUCCESS ){
													endp ^= 0x80;                                               // ͬ����־��ת
                          memset(TxBuffer,0,sizeof(TxBuffer));
                          UH_TX_LEN = 64;                                             // Ĭ�������ݹ�״̬�׶�ΪIN										
											    s = USBHostTransact( USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff );// CH554��������,��ȡ����,NAK����   											
											}
											else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) ) printf("Printer error %02x\n",(UINT16)s); // �����ǶϿ���
											}
									}
							}				
            break;
        default:
            break;
        }
    }
}