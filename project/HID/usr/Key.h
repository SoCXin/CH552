
#define INTERRUPT_TouchKey   0   //开启Key中断方式


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

#define MOUSE 0


/*******************************************************************************
* Function Name  : TouchKeyQueryCylSet(UINT8 cyl)
* Description    : 触摸按键查询周期设置
* Input          : UINT8 cyl 时钟设置                  								
                   1   2ms
                   0   1ms									 
* Output         : None 
* Return         : None
*******************************************************************************/
void TouchKeyQueryCylSet1Or2ms(UINT8 cyl);

/*******************************************************************************
* Function Name  : TouchKeyChanelSelect(UINT8 ch)
* Description    : 触摸按键通道选择
* Input          : UINT8 ch 采用通道
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL
*******************************************************************************/
UINT8 TouchKeyChanelSelect(UINT8 ch);

