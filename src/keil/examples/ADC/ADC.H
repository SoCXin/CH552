
#define ADC_INTERRUPT   1 

/*******************************************************************************
* Function Name  : ADCClkSet(UINT8 div)
* Description    : ADC采样时钟设置,模块开启，中断开启
* Input          : UINT8 div 时钟设置 
                   0 慢  384个Fosc                   								
                   1 快  96个Fosc									 
* Output         : None
* Return         : None
*******************************************************************************/
void ADCInit(UINT8 div);

/*******************************************************************************
* Function Name  : ADC_ChannelSelect(UINT8 ch)
* Description    : ADC采样通道设置
* Input          : UINT8 ch 采用通道0-3
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL 通道设置超出范围
*******************************************************************************/
UINT8 ADC_ChannelSelect(UINT8 ch);

/*******************************************************************************
* Function Name  : VoltageCMPModeInit()
* Description    : 电压比较器模式初始化
* Input          : UINT8 fo 正向端口 0\1\2\3
                   UINT8 re 反向端口 1\3
* Output         : None
* Return         : 成功 SUCCESS
                   失败 FAIL
*******************************************************************************/
UINT8 VoltageCMPModeInit(UINT8 fo,UINT8 re);
