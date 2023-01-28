# [CH552](https://doc.soc.xin/CH552)

[![Build Status](https://github.com/SoCXin/CH552/workflows/demo/badge.svg)](https://github.com/SoCXin/CH552/actions/workflows/demo.yml)
[![Build Status](https://github.com/SoCXin/CH552/workflows/src/badge.svg)](https://github.com/SoCXin/CH552/actions/workflows/src.yml)

* [WCH](http://www.wch.cn/)：[MCS-51](https://github.com/SoCXin/MCS-51)
* [L1R1](https://github.com/SoCXin/Level)：24 MHz

## [简介](https://doc.soc.xin/CH552)

[CH552](http://www.wch.cn/products/CH552.html) 兼容MCS51的增强型E8051内核，79%指令是单字节单周期指令，2KB BootLoader + 14KB Flash，1K xRAM(支持DMA) +256B iRAM，128B DataFlash。

内置24MHz时钟源和PLL，内置了ADC模数转换、触摸按键电容检测、3组定时器和信号捕捉及PWM、双异步串口、SPI、USB设备控制器和全速收发器、USB type-C等功能模块。

### 关键参数

* 24 MHz MCS-51 Core
* 256B IRAM + 1KB SRAM + 14KB FLASH + 128B EEPROM
* 5V->3.3V LDO
* 2KB BootLoader，支持USB和串口ISP
* Type-C 主从检测
* USB device fs
* UART x 2 + SPI x 1
* 小封装 (TSSOP20/SOP16/MSOP10/QFN16)

## [资源收录](https://github.com/SoCXin)

* [参考资源](src/)
* [参考文档](docs/)
* [参考工程](project/)

## [选型建议](https://github.com/SoCXin/PL51RC003)

[CH552](https://item.szlcsc.com/112593.html) 在USB应用领域拥有较高的人气，低阶高性价比的USB单片机。可以使用平台[platform-wch51](https://github.com/OS-Q/platform-wch51)进行开发，支持官方库和arduino

所属同系列中[CH554](https://github.com/SoCXin/CH554) 增加支持USB HOST主机模式。[CH551](http://www.wch.cn/products/CH551.html)的ROM降为10K，片内xRAM为512字节，异步串口仅提供UART0，仅SOP16封装，并且去掉了ADC模数转换模块和USB type-C模块。

相较[STM8S003](https://github.com/SoCXin/STM8S003) 及 [N76E003](https://github.com/SoCXin/N76E003) 系列产品，[CH552](https://github.com/SoCXin/CH552)缺少IIC硬件，ADC较弱，没有专门的 eeprom存储器。TSSOP20封装不兼容前两者，无法直接替换。

## [www.SoC.xin](http://www.SoC.Xin)
