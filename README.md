# [CH552](https://github.com/SoCXin/CH552)

[![sites](http://182.61.61.133/link/resources/SoC.png)](http://www.SoC.Xin)

#### [Vendor](https://github.com/SoCXin/Vendor)：[WCH](http://www.wch.cn/)
#### [Core](https://github.com/SoCXin/8051)：[8051](https://github.com/SoCXin/8051)
#### [Level](https://github.com/SoCXin/Level)：24MHz

## [简介](https://github.com/SoCXin/CH552/wiki)

[CH552](https://github.com/SoCXin/CH552) 兼容MCS51的增强型E8051内核，79%指令是单字节单周期指令，2KB BootLoader + 14KB Flash，1K xRAM(支持DMA) +256B iRAM，128B DataFlash。

内置24MHz时钟源和PLL，内置了ADC模数转换、触摸按键电容检测、3组定时器和信号捕捉及PWM、双异步串口、SPI、USB设备控制器和全速收发器、USB type-C等功能模块。

[![sites](docs/CH552.png)](http://www.wch.cn/products/CH552.html)
#### 关键特性

* 5V->3.3V LDO
* 2KB BootLoader，支持USB和串口ISP
* Type-C 主从检测
* USB device fs
* UART x 2 + SPI x 1
* 小封装 (TSSOP20/SOP16/MSOP10/QFN16)

### [资源收录](https://github.com/SoCXin/CH552)

* [参考文档](docs/)
* [参考资源](src/)
* [参考工程](project/)
* [下载工具](http://www.wch.cn/downloads/WCHISPTool_Setup_exe.html)

### [选型建议](https://github.com/SoCXin)

[CH552](https://github.com/SoCXin/CH552) 在8051领域拥有较高的声望，低阶高性价比USB单片机。

所属同系列中[CH554](https://github.com/SoCXin/CH554) 增加支持USB HOST主机模式。[CH551](http://www.wch.cn/products/CH551.html)的ROM降为10K，片内xRAM为512字节，异步串口仅提供UART0，仅SOP16封装，并且去掉了ADC模数转换模块和USB type-C模块。

相较[STM8S003](https://github.com/SoCXin/STM8S003) 及 [N76E003](https://github.com/SoCXin/N76E003) 系列产品，[CH552](https://github.com/SoCXin/CH552)缺少IIC硬件，ADC较弱，没有专门的 eeprom存储器。TSSOP20封装不兼容前两者，无法直接替换。

###  [探索芯世界 www.SoC.xin](http://www.SoC.Xin)
