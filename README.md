# CarOS：小车驱动程序及应用程序接口

## :book: 简介

### 当前版本v1.3.1

**CarOS是什么**

CarOS是针对于电赛(使用STM32及MSP432)开发的小车专用的一套底层驱动程序及浅层应用程序接口，能够满足用户在其上开发一些应用程序而减少对于驱动的接触，帮助用户在最短时间内搭建一辆小车。

**CarOS不是什么**

CarOS不是操作系统(Operating System)，它还需要依赖其他实时操作系统，如FreeRTOS等才能正常发挥作用。在未来会考虑封装FreeRTOS进入CarOS而使CarOS变成基于FreeRTOS的小车操作系统，这也是该项目以OS为名的一个原因。

## :rocket: 特性

**高可移植性**

只需配置很少一部分代码即可在STM32或MSP432平台上运行。

**高可扩展性**

可扩展直流电机驱动、直流电机速度环控制算法、车体运动学闭环控制算法，且均留有标准接口和扩展教程。

**便于调试**

自带串口控制台，可使用命令控制小车；自带小车日志记录，可及时了解小车异常情况
```
load app "dmesg"
load app "smv"
load app "shmpid"
load app "smpid"
load app "scv"
load app "mpidsa"
load app "pkg"
Console ready.
debug init done.
Left Front Motor initialization done.
Left Front Motor adding PID velocity controller succeed.
Left Front Motor has mounted L298N driver.
Left Rear Motor initialization done.
Left Rear Motor adding PID velocity controller succeed.
Left Rear Motor has mounted L298N driver.
Right Front Motor initialization done.
Right Front Motor adding PID velocity controller succeed.
Right Front Motor has mounted L298N driver.
Right Rear Motor initialization done.
Right Rear Motor adding PID velocity controller succeed.
Right Rear Motor has mounted L298N driver.
car set move towards 0.00degree at speed 40.00cm/s, at angular velocity 0.00rad/s.
car set move towards 10.00degree at speed 20.00cm/s, at angular velocity 4.00rad/s.
```

**模块可拆卸**

只需删减部分代码，各模块均可拆卸出来单独使用。

**注释详尽**

代码带有大量注释，便于用户了解实现原理，这也是为了希望用户能多提出代码的弊病和不足之处。

## :robot: 开始上手

[开始上手](https://github.com/Pansamic/CarOS/blob/master/docs/%E7%94%A8%E6%88%B7%E6%8C%87%E5%8D%97/CarOS-user%20manual.md)

### 帮助

对于各个模块的说明及帮助都写在对应模块的文档中了（有一些尚未写完），用户可以查看对应的文档寻找帮助。

如有问题或者对CarOS的建议，请联系邮箱`pansamic@foxmail.com`

欢迎各位为CarOS添加更多驱动、更多小车模型、更多上层应用程序~~

## :earth_asia: 当前支持

### 小车模型

1. 4轮麦克纳姆轮小车模型

### 电机速度环算法

1. PID算法

### 直流电机驱动

1. L298N模块

### 串口命令控制台

PC机使用串口调试工具向小车发送命令以使小车执行命令相关程序

## :bookmark_tabs: 未来计划

### v1.4

1. 小车模型加入对姿态传感器的通用接口
2. 增加姿态传感器MPU6050驱动
3. 增加A4950直流电机驱动

### v1.5

1. 全面支持MSP432
2. 增加姿态传感器ICM20640的驱动

### v1.6
1. 增加OpenMV的驱动及少量应用程序

### v1.7

1. 加入SPI-Flash芯片驱动
2. 提供将日志存储至SPI-Flash的选项

### v2.0

1. 封装FreeRTOS

## :open_file_folder: 仓库结构

`CarOS/src`：源代码文件夹，存放了CarOS驱动库。

`CarOS/docs`：文档文件夹，存放了CarOS的各类文档。

`CarOS/demo`：样例文件夹，存放了使用CarOS的直接可使用样例。

## :calendar: 版本更新记录

[版本更新记录](https://github.com/Pansamic/CarOS/blob/master/docs/develop.md)