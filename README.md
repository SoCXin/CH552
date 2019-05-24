# [CH552](https://github.com/sochub/CH552) 
[![sites](SoC/SoC.png)](http://www.qitas.cn) 
####  qitas@qitas.cn
#### 父级：[CH55系列](https://github.com/sochub/CH55)
#### 归属：[8051](https://github.com/sochub/8051) 

## [芯片简介](https://github.com/sochub/CH552/wiki)

CH552芯片是一款兼容MCS-51指令集的增强型E8051内核单片机，其79%的指令是单字节单周期指令，平均指令速度比标准MCS-51快8～15 倍。

CH552 支持最高24MHz 系统主频，内置16K 程序存储器ROM 和256 字节内部iRAM 以及1K 字节片内xRAM，xRAM 支持DMA直接内存存取。

CH552内置了ADC 模数转换、触摸按键电容检测、3 组定时器和信号捕捉及PWM、双异步串口、SPI、USB设备控制器和全速收发器、USB type-C等功能模块。

[![sites](SoC/CH552.png)](http://www.wch.cn/products/CH552.html) 

### [资源组成](https://github.com/sochub/CH552)

* [参考文档](docs/)
	* [芯片手册](docs/)
* [参考资源](src/)
	* [引用文件](src/include)
	* [make工程](src/make/)
	* [keil工程](src/keil/)
		* [参考资源](src/examples)
		* [LCD设备](demo/LCD)
		* [HID设备](demo/HID)
	
### [选型建议](https://github.com/sochub/CH552)

CH552芯片是WCH 8051 USB单片机中的低成本方案，性价比十分高，在需要USB和上位机通信的场合使用非常便利（例如模拟WCH家的CH340串口通信）。

相同规格略提升替换可选[CH554](https://github.com/sochub/CH554)，更高性能的替换可选2倍左右性能和资源增幅的[CH558](https://github.com/sochub/CH558) 或 [CH559](https://github.com/sochub/CH559)，后者可以提供如LQFP48等封装规格。

更多功能接口或无线集成可选[CH56系列](https://github.com/sochub/CH56)或[CH57系列](https://github.com/sochub/CH57)替换，同厂方案。


##  [SoC资源平台](http://www.qitas.cn)
