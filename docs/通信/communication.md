# 通信模块说明

## 目录
[1. 简介](#1-简介)</br>
[2. 模块开发前期对于通信模块的想法](#2-模块开发前期对于通信模块的想法可忽略)</br>
[3. 通用输入输出设备](#3-通用输入输出设备)</br>
----[3.1 通用输入输出设备简介](#31-通用输入输出设备简介)</br>
----[3.2 接口说明](#32-接口说明)</br>
----[3.3 使用方法](#33-使用方法)</br>
--------[3.3.1 系统内置io设备及命令控制台](#331-系统内置io设备及命令控制台)</br>
--------[3.3.2 自定义io设备](#332-自定义io设备)</br>
----[3.4 代码分析](#34-代码分析)</br>
--------[3.4.1 io结构体](#341-io结构体)</br>
--------[3.4.2 对于输入的处理](#342-对于输入的处理)</br>
--------[3.4.3 对于输出的处理](#343-对于输出的处理)</br>
----[3.5 可移植性](#35-可移植性)</br>
----[3.6 可改进的方面](#36-可改进的方面)</br>
[4. 命令解释器](#4-命令解释器)</br>
----[4.1 命令解释器简介](#41-命令解释器简介)</br>
----[4.2 接口说明](#42-接口说明)</br>
----[4.3 使用方法](#43-使用方法)</br>
----[4.4 代码分析](#44-代码分析)</br>
[5. 数据包处理器](#5-数据包处理器)</br>
----[5.1 简介](#51-简介)</br>
----[5.2 接口说明](#52-接口说明)</br>
----[5.3 使用方法](#53-使用方法)</br>
----[5.4 代码分析](#54-代码分析)</br>
--------[5.4.1 数据包格式结构体](#541-数据包格式结构体)</br>
--------[5.4.2 数据包解析过程](#542-数据包解析过程)</br>
[6. 模块拆卸指导](#6-模块拆卸指导)</br>


## 1. 简介

CarOS通信模块基于MCU的串口。

通信模块实现了使用电脑串口调试工具向MCU发送带有数值的命令（字符串）后，部署在MCU的命令解释器解释命令并从字符串中获取参数值，调用并传参给该命令所对应的函数，以此来实现类似于命令行的功能。

该模块可以经过简单的删减代码之后移植到任何MCU甚至PC机上，所以可以很方便地移植至ESP32、ESP8266等设备。

## 2. 模块开发前期对于通信模块的想法(可忽略)

**系统命令**

* 主要为人机交互通信，其次为小车协同通信
* 人机交互通信就不能用数据包的形式，因为人不可能记得各种各样的数据码，所以必须用字符串的形式。
* 要实现用户自定义通信，开发人员要做的就是定义好通信数据格式，然后用户自定义数据内容，我所定义好一个结构体，里面有1.命令标识2.是否含有数据3.数据格式4.数据内容5.执行函数，这样一来就能让用户自定义命令了
* 整个系统内部当然也要自带一套核心命令，就像linux内置的应用一样
* 那么用户在使用CarOS开发应用程序的时候就基本上只用写一个命令集结构体还有一些函数就可以了，头文件就include一下cos.h就行，驱动程序都在cos里面写好了
```c
typedef struct COS_Cmd
{
    const char *CmdName;
    uint8_t ContainData;
    
}COS_Cmd;
```

**通用输入输出**
* 为了解决阻塞式输入输出（尤其是printf）速度很慢而且相当耗费单片机资源的问题，非常有必要采用缓冲区输入输出的方式，在程序正常运行的时候把输出数据写入缓冲区，在程序闲置的时候把缓冲区数据输出到串口，在这其中就非常有必要把单片机的DMA好好利用起来。
* 输入输出设备就是串口和一些缓冲区还有一些读写缓冲区的函数而已，这些东西打包起来形成一个输入输出设备。
* 系统自带一个输入输出设备，但是留出通用输入输出定义的接口给用户自己设置输入输出设备，比方说给其他车子的通信设备
* 系统自带的输入输出设备应该适配STM32的HAL库和固件库以及MSP432，以避免用户还要自己实现系统输出函数。
* 通用输出应该提供写缓冲区的函数，在需要输出的时候直接写缓冲，在空闲的时候调用缓冲区输出函数
* 千万注意挂在io设备上的缓冲区一定不能不能是在某个函数里面定义的局部变量，一定要是全局变量！
* 发送应该使用双缓冲模式，考虑到不能不使用C标准库的格式化字符串函数，而恰好C标准格式化字符串函数没有好办法向环形缓冲区写入任意长度的数据。
* 采用双缓冲发送、环形缓冲区接收。双缓冲第一缓冲区比较大，第二缓冲区比较小。
* 双缓冲发送的逻辑：1.活动缓冲区写入一次之后，把另外一个缓冲区设置为活动缓冲区。2.如果剩余缓冲区大小不足以存放该次数据，那就把缓冲区填满并直接舍弃剩下的数据。3。如果发现第一缓冲区当前是发送区，而且第一缓冲区为空，则把第一缓冲区设置为活动区。

## 3. 通用输入输出设备

### 3.1 通用输入输出设备简介

通用输入输出设备是基于串口的一套系统，实际上是一个**结构体**。通用输入输出设备主要实现了电脑与小车、小车与小车、小车与其他串口设备之间的通信。这样就能够实现诸如电脑串口**命令行**终端控制小车、**两车协同**、**MSP432-ESP32-STM32通信**等等功能。

输入输出均有缓冲区，输入缓冲区是一个环形缓冲区，输出采用两个缓冲区组成的乒乓缓冲模式，缓冲区的存在可以让MCU在空闲（使用实时操作系统才有空闲态）的时候再处理输入输出信息而不会影响运动学算法的正常运行。在输出方面，通过缓冲区+**DMA**的模式，可以大大降低**传统printf**函数阻塞式输出造成的**卡顿**，卡顿在小车运动学计算时可能是致命的。在输入方面，输入的数据可以使用[命令解释器](#4-命令解释器)来处理，也可以使用[数据包解析器](#5-数据包处理器)来处理，也可以使用字符串模式接收一行文本。命令解释器主要处理字符串，数据包解析器主要用于处理接收的二进制数据。这3种模式被称为命令模式和数据包模式和字符串模式，3种模式可以通过函数相互切换。

### 3.2 接口说明

#### :round_pushpin: io设备初始化
```c
void io_Init(COS_io *ioDevice, 
             const char* Name,
             COS_uart huart,
             uint8_t *InputBuf,
             uint32_t InputBufLen,
             uint8_t *OutputBuf1,
             uint32_t OutputBuf1Len,
             uint8_t *OutputBuf2,
             uint32_t OutputBuf2Len)
```

**简介**

向CarOS添加一个io设备并将函数参数传递给该io设备。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **Name**:该io设备的名称。
3. **huart**:指向串口实例的指针。
4. **InputBuf**:输入缓冲区指针。
5. **InputBufLen**:输入缓冲区大小。
6. **OutputBuf1**:输出缓冲区1的指针。
7. **OutputBuf1Len**:输出缓冲区1的大小。
8. **OutputBuf2**:输出缓冲区2的指针。
9. **OutputBuf2Len**:输出缓冲区2的大小。

**使用示例**

```c

uint8_t COS_InputBuffer[1024*INPUT_BUFFER_SIZE];
uint8_t COS_OutputBuffer1[1024*OUTPUT_BUFFER1_SIZE];
uint8_t COS_OutputBuffer2[1024*OUTPUT_BUFFER2_SIZE];
io_Init(&cosio, "cosio", &huart2, COS_InputBuffer, INPUT_BUFFER_SIZE*1024, COS_OutputBuffer1, OUTPUT_BUFFER1_SIZE*1024, COS_OutputBuffer2, OUTPUT_BUFFER2_SIZE*1024);
```

#### :round_pushpin: 处理所有io设备
```c
void io_Process()
```

**简介**

由于所有的io设备都会挂载到`COS_io* _iob[6]`这个挂载点上，所以该函数遍历所有io设备并且处理其输入输出。

**参数列表**

无参数

**使用示例**
在STM32CubeIDE生成的FreeRTOS任务函数中周期性地调用该函数。
```c
void CarOS_ioProcess(void const * argument)
{
  /* USER CODE BEGIN CarOS_ioProcess */
  /* Infinite loop */
  for(;;)
  {
    io_Process();
  }
  /* USER CODE END CarOS_ioProcess */
}
```


#### :round_pushpin: io设备格式化输出
```c
void io_printf(COS_io *ioDevice,const char *fmt,...)
```

**简介**

该函数将格式化字符串数据写入io设备的输出缓冲区，至于最终的串口发送由`io_OutputProcess()`函数完成。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **fmt**:带有格式化标记(如%d)的字符串。
3. 可选参数:格式化字符串中对应的参数。


#### :round_pushpin: CarOS内置格式化输出
```c
void COS_printf(const char *fmt,...)
```

**简介**

该函数可以替代printf()函数，但前提是配置好系统io，配置方法详见CarOS用户手册。

该函数实现的功能是把格式化字符串写入到io设备的输出缓冲区中，至于最终的串口发送由`io_OutputProcess()`函数完成。

**参数列表**

1. **fmt**:带有格式化标记(如%d)的字符串。
2. 可选参数：格式化字符串中的参数。

**使用示例**

```c
COS_printf("print %d, %s",8,"hello!");
```

#### :round_pushpin: io设备发送数据
```c
void io_SendData(COS_io *ioDevice, uint8_t *pData, uint32_t Length);
```

**简介**

该函数将待发送的数据写入io设备发送缓冲区，至于最终的串口发送由`io_OutputProcess()`函数完成。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **pData**:待发送数据的指针。
3. **Length**:待发送数据的长度。

**使用示例**

```c
io_SendData(&ESPio,"AT+CWJAP=\"ssid\",\"password\"");
```

#### :round_pushpin: io设备传输完成处理函数
```c
void io_TransOverHandler(COS_uart huart)
```

**简介**

该函数作用是清除io设备“正在发送”的标志位，以让io设备准备好下一次发送。如果在c_config.h中开启DMA传输，则该函数应该被放在DMA传输完成中断中。

**参数列表**

1. huart:串口指针

**使用示例**
```c
/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
    /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

    /* USER CODE END DMA1_Stream6_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_usart2_tx);
    /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */
    io_TransOverHandler(&huart2);
    /* USER CODE END DMA1_Stream6_IRQn 1 */
}
```

#### :round_pushpin: 切换字符串模式
```c
void io_StringMode(COS_io *ioDevice, void(*Stringcb)(char*));
```

**简介**

该函数切换io设备输入模式为字符串模式，每获取到一行文字就执行一次回调函数。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **Stringcb**:回调函数指针。

**使用示例**
```c
io_StringMode(&ESPio,ESP_StrProcesscb);
void ESP_StrProcesscb(char* Str)
{
  ...
}
```

### 3.3 使用方法

#### 3.3.1 系统内置io设备及命令控制台

##### 1. 设置控制台串口

打开`CarOS/src/communication/c_communication.c`文件，第33行左右修改cosio所使用的串口。

```c
#if USE_HAL_LIB
#define cosiohuart huart2 // only need to modify here.
extern UART_HandleTypeDef cosiohuart;
#elif USE_FW_LIB
/* notice that USARTx is pointer rather than  */
#define cosiouart USART1 // only need to modify here.
extern USART_TypeDef* cosiouart;
#elif USE_MSP432
#define cosiohuart EUSCI_A0_BASE
#endif
```

##### 2. 部署io设备管理函数

若使用FreeRTOS或其他实时操作系统，请设置空闲时调用`io_Process()`函数。以FreeRTOS为例：

```c
void CarOS_ioProcess(void const * argument)
{
  /* USER CODE BEGIN CarOS_ioProcess */
  /* Infinite loop */
  for(;;)
  {
    io_Process();
  }
  /* USER CODE END CarOS_ioProcess */
}
```

##### 3. 初始化cosio

在`main()`函数中，初始化MCU硬件之后，请调用`CarKernelInit()`函数来初始化系统，其中就包含了系统内置io设备的初始化。

```c
int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  CarKernelInit();    /* CarOS系统初始化 */
  ...
}
```

##### 4. 使用COS_printf()格式化输出

在进行了以上部署之后，用户就可以使用CarOS提供的格式化输出了。在程序的任何地方都可以使用COS_printf()来打印格式化字符串。

##### 5. PC机串口发送命令控制CarOS

系统内置了一些命令，第一种内置命令是系统级，第二种内置命令是针对不同小车而不同的。系统级命令在`CarOS/src/app/c_builtinapp.c`中。小车的命令在`CarOS/src/car/c_xwheel_xxxxx.c`中，不同模型的小车有不同的命令。关于命令的具体说明请见[命令解释器](#4-命令解释器)

#### 3.3.2 自定义io设备

自定义io设备主要用于MCU和其他具有串口的设备进行通信，也可以自己再定义一个io设备用来跟PC机交换数据，或者，给小车加一个蓝牙串口连接手机使用蓝牙遥控等等。

##### 1. 声明并定义COS_io类型的全局变量

在`main.c`中定义一个全局变量

##### 2. 声明并定义缓冲区

缓冲区的类型都是`uint8_t`类型的数组，大小不宜小于128字节。

##### 3. 编写io设备应用程序（可选）

以COS_App类型的结构体数组作为一个App列表，详细关于应用程序的说明请见`CarOS/docs/命令及应用程序/`文件夹

##### 3. 初始化io设备

调用`io_Init()`函数初始化。

##### 4. 使用io_printf()输出格式化字符串

最终的模板是这样的：

```c
#define MYAPPAMOUNT 1
COS_io XXXio; // 名字可以自己随便取
uint8_t XXX_InputBuffer[256];
uint8_t XXX_OutputBuffer1[512];
uint8_t XXX_OutputBuffer2[256];
/* 如果该io设备不用命令模式可以不写AppList */
COS_App myAppList[MYAPPAMOUNT]={
    {"xxx","xxx,xxx,xxx",xxxxcb}
};
int main()
{
    ...
    /* 使用STM32 HAL 库则按以下方式传递串口，注意是否对串口取地址 */
    io_Init(&XXXio, "XXXio", &huart2, XXX_InputBuffer, 256, XXX_OutputBuffer1, 512, XXX_OutputBuffer2, 256);

    /* 使用STM32 固件库则按以下方式传递串口，注意是否对串口取地址 */
    // io_Init(&XXXio, "XXXio", USART2, XXX_InputBuffer, 256, XXX_OutputBuffer1, 512, XXX_OutputBuffer2, 256);

    /* 使用MSP432则按以下方式传递串口，注意是否对串口取地址 */
    // io_Init(&XXXio, "XXXio", EUSCI_A0_BASE, XXX_InputBuffer, 256, XXX_OutputBuffer1, 512, XXX_OutputBuffer2, 256);

    LoadApp(&XXXio, myAppList, MYAPPAMOUNT);
    ...

    COS_printf("print name: %s",XXXio.Name);
}
void 串口中断服务函数()
{
    static uint8_t ReceivedData;
    ...
    io_UartRxIntHandler(&huart2);
    // io_UartRxIntHandler(USART2); (STM32固件库)
    // io_UartRxIntHandler(EUSCI_A0_BASE);(MSP432)
    ...
}
void DMA传输完成中断服务函数()
{
    ...
    io_TransOverHandler(&huart2);
    // io_TransOverHandler(USART2); (STM32固件库)
    // io_TransOverHandler(EUSCI_A0_BASE);(MSP432)

    ...
}
```
### 3.4 代码分析

#### 3.4.1 io结构体

```c
typedef struct ioDeviceDefinition
{
	const char *Name;
	COS_uart    huart;
	/* double buffer */
	uint8_t     ActivatedOutBufIndex;
	uint8_t     Outputing;
	uint8_t    *OutputBuf[2];
	uint32_t    OutputBufSize[2];
	uint32_t    OutputBufPosition[2];


	/* input buffer related */
	uint8_t    *InputBuf;
	uint32_t    InputBufSize;
	uint32_t    InputHeadIndex;
	uint32_t    InputTailIndex;
	uint8_t     InputBufFull;

	uint8_t     ReceiveMode;
	void      (*Stringcb)(char*);
	COS_Cmd    *CmdList;
	COS_PkgPrc *PkgProcessor;
}COS_io;
```

**结构体成员**

1. **Name**:io设备名称，字符串。
2. **huart**:发送和接收数据的串口的指针，依MCU驱动库不同而不同，对于STM32-HAL库来说，是`&huart2`这种格式；对于STM32固件库来说，是`USART2`这种格式；对于MSP432的驱动程序库来说，是`EUSCI_A0_BASE`这种格式。详见COS_uart类型的定义。
3. **ActivatedOutBufIndex**:当前输出活动缓冲区的下标，对于双缓冲来说，值为0或1。
4. **Outputing**:当io设备正在发送数据时值为1，当io设备未发送数据时值为0。
5. **OutputBuf**:2个元素的指针数组，用来指向两个输出缓冲区。
6. **OutputBufSize**:2个元素的数组，用于保存两个输出缓冲区的大小。
7. **OutputBufPosition**:2个元素的数组，用于保存两个输出缓冲区当前已有数据的大小。
8. **InputBuf**:指向输入缓冲区的指针。
9. **InputBufSize**:输入缓冲区的空间大小。
10. **InputHeadIndex**:环形缓冲区的头部下标。
11. **InputTailIndex**:环形缓冲区的尾部下标。
12. **InputBufFull**:标志位，当输入缓冲区满时为1，未满时为0。
13. **ReceiveMode**:标志位，用于指示当前io设备的接收模式是[命令模式](#342-命令模式和数据包模式)还是[数据包模式](#342-命令模式和数据包模式)
14. **Stringcb**:字符串模式下的回调函数。每从输入缓冲区获取到一行（以'\n'结尾的字符串）后将该字符串作为参数传递到回调函数中并执行回调函数。
15. **CmdList**:`COS_Cmd`类型的链表头指针，仅当io设备工作在命令模式有效。
16. **PkgProcessor**:数据包模式下用于保存数据包工作状态的结构体的指针

**缓冲区结构的设计思路**

一开始尝试把输入输出缓冲区都设置成环形缓冲区，但是发现使用环形缓冲区作为输出缓冲区有很多弊病，列举如下：

1. 使用DMA发送时，数据若从接近缓冲区空间尾部开始，经过缓冲区空间末尾，再回到缓冲区空间头部，则数据会被分存储在两块不连续的内存中，这样会导致DMA在发送时需要发送两次，而这其中还需要设置很多标记和逻辑判断来保证数据被完全发送。
2. 在DMA发送时，不能操作DMA发送的那一部分内存空间的数据，否则会进入Hard Fault，所以在发送和写入两个操作同时存在时，容易造成数据混淆。
3. 在使用`vsnprintf()`函数向输出缓冲区写入格式化字符串时，如果写入地址接近缓冲区内存空间末尾，而且待写入数据量较大时，由于C语言不能自动把缓冲区末尾地址的下一个地址跳转到缓冲区内存空间起始地址，只能舍弃待写入的剩余数据而把缓冲区填满。然而这样就造成了数据的大量失真。

所以最终采用了双缓冲发送。由于传统双缓冲的两块内存空间大小是一样的，但单片机RAM资源又很紧俏，如果设置两块超大内存空间作为双缓冲，对于io设备来说显得有点浪费，因为只有当发送速度非常快，比如发送屏幕内容数据时才需要两块都很大的双缓冲，而io设备的发送特点是发送频率低、一次性发送数据一般很少但有时又特别多（发送[日志](https://github.com/Pansamic/CarOS/blob/master/docs/调试/debug.md)）。所以作者稍稍改进了一下双缓冲发送模式。这里采用主副双缓冲的模式，主缓冲区的内存空间可以设置得稍微大一些，副缓冲区的内存空间可以设置得小一些，闲置时发现主缓冲区空闲就激活主缓冲区，这样可以节省一些内存。

#### 3.4.2 对于输入的处理

##### 命令模式、数据包模式和字符串模式

这3个模式是针对接受到的数据的两种不同处理方式。通过`io_CommandMode()`或`io_PackageMode()`或`io_StringMode()`函数来切换io设备的数据处理器。具体详见[命令解释器](#4-命令解释器)和[数据包处理器](#5-数据包处理器)

#### 3.4.3 对于输出的处理

若程序调用`io_printf()`（对于系统内置io设备来说是`COS_printf()`）或`io_SendData()`或`io_SendDataPackage()`这种输出函数后，数据会被写入当前激活的缓冲区中（因为输出缓冲区有两个），在下一次调用`io_Process()`函数之后，活动缓冲区中的数据会被串口发送出去，并且锁定这个缓冲区、将另外一个输出缓冲区设置为活动缓冲区。

### 3.5 可移植性

可以移植至很多MCU上。对于输出，只需要改`_io_Transmit()`函数就可以了。对于输入，需要修改COS_uart类型的定义、`io_UartRxIntHandler()`函数。

### 3.6 可改进的方面

可以增加其他通信外设如SPI、I2C等通信外设，这样的话，io设备的意义就不只是命令行了。这样一来很多外设都可以使用io设备的接口，每一个io设备都对应一个外设，这样可以方便集中统一管理所有外设。

但其实想一想并没有必要，因为i2c这种外设是采用查询方式读取数据的，它不会主动发送数据，因此输入缓冲区就没用了。

## 4. 命令解释器

### 4.1 命令解释器简介

命令解释器获取io设备输入缓冲区中带有命令的字符串并且按照字符串解析出命令名称和参数，然后在io设备挂载的应用程序列表中查找对应的命令并且调用该嘤嘤程序的

### 4.2 接口说明

#### :round_pushpin: 设置io设备为命令模式
```c
void io_CommandMode(COS_io *ioDevice)
```

**简介**

将io设备结构体成员`ReceiveMode`设置为`COMMAND_MODE`,重置io设备结构体成员`PkgProcessor`。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。

**使用示例**

```c
io_CommandMode(&cosio);
```

#### :round_pushpin: 为io设备添加命令
```c
void io_AddCmd(COS_io *ioDevice, const char *CmdName, const char *ParamsDescription, void* CmdCallbackFunc)
```

**简介**

这个函数是跟应用程序层的接口，应用程序通过这个函数来一次性给io设备挂载多个命令。该函数生成一个COS_Cmd结构体并且挂载至COS_io结构体的CmdList成员上。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **CmdName**:命令名称
3. **ParamsDescription**:一个字符串，描述该命令的参数类型，如："int float float uint8_t"。每个类型之间用空格隔开。
4. **CmdCallbackFunc**:命令回调函数指针。

**使用示例**

```c
io_AddCmd(ioDevice, "scv", "float float float", SetCarVelocity);
```


### 4.3 使用方法

使用`io_AddCmd()`函数为io设备挂载命令。按照用户手册中的系统配置方法配置好系统后，程序会自动解析接收到的字符串并且调用命令对应的回调函数。
详见[用户手册](https://github.com/Pansamic/CarOS/blob/master/docs/%E7%94%A8%E6%88%B7%E6%8C%87%E5%8D%97/CarOS-user%20manual.md)和[命令及应用程序](https://github.com/Pansamic/CarOS/blob/master/docs/%E5%91%BD%E4%BB%A4%E5%8F%8A%E5%BA%94%E7%94%A8%E7%A8%8B%E5%BA%8F/application.md)

### 4.4 代码分析


## 5. 数据包处理器

### 5.1 简介

数据包解析器能够以自定义格式自动解析数据包并抽取原始数据作为参数传递给回调函数。这为用户提供了开发某些使用串口发送数据包的传感器的解决方案。

### 5.2 接口说明

#### :round_pushpin: 设置io设备为数据包输入模式
```c
void io_PackageMode(COS_io *ioDevice, uint8_t FormatEnable, uint8_t RawDataSize, void *CallbackFunc)
```

**简介**

将io设备结构体成员`ReceiveMode`设置为`PACKAGE_MODE`，初始化io设备成员`PkgProcessor`。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **FormatEnable**:设置数据包传输模式是否启用数据包格式解析。
3. **RawDataSize**:单次接收的数据包中的原始数据的大小。单位：字节
4. **CallbackFunc**:接收到数据包后要自动调用的函数的指针。

**使用示例**

```c
/* Radario 开启数据包模式，开启数据校验，数据包大小128字节
 * 收到数据后的回调函数是 RadarProcess() */
io_PackageMode(&Radario,1,128,RadarProcess);

void RadarProcess(uint8_t* DataPkg)
{
    ...
}
```
#### :round_pushpin: 设置io设备数据包解析格式
```c
void io_SetPkgParseFmt(COS_io *ioDevice, COS_PkgFmt *PkgFmt)
```

**简介**

该函数必须在io设备已经设置为数据包格式之后调用。该函数设置io设备以PkgFmt结构体指定的格式自动解析从串口发送来的数据包。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **PkgFmt**:指向数据包解析格式结构体的指针。

**使用示例**

```c
const uint8_t SensorPkgHead[2]={0x5A,0X3B};
const COS_PkgFmt SensorPkgFmt=
{
    1,               // enable package head
    SensorPkgHead,   // use sensor head sequence
    2,               // head sequence length equals two
    1,               // enable data check
    Chk_XOR,         // use XOR check algorithm
    0,               // disable tail
    NULL,            // use null tail sequence
    0,               // tail sequence length equals 0
    0                // unnecessary value, because tail is disabled
};
io_SetPkgParseFmt(&sensorio, &SensorPkgFmt);
```

#### :round_pushpin: 发送数据包
```c
void io_SendDataPackage(COS_io *ioDevice, void *pData, uint32_t Length, COS_PkgFmt *PkgFmt)
```

**简介**

除了原始数据之外，在原始数据前面加上了两个数据包头字节，在原始数据为不加上了一个校验字节，然后把整个数据包写入输出缓冲区。

**参数列表**

1. **ioDevice**:指向COS_io结构体的指针。
2. **pData**:待发送的数据的指针。
3. **Length**:待发送的数据的大小。
4. **PkgFmt**:指向PkgFmt结构体的指针，PkgFmt包含了数据包的解析格式。

**使用示例**

```c
const COS_PkgFmt DefaultPkgFmt=
{
    1,               // enable package head
    DefaultPkgHead,  // use default head sequence
    2,               // head sequence length equals two
    1,               // enable data check
    Chk_XOR,         // use XOR check algorithm
    0,               // disable tail
    NULL,            // use null tail sequence
    0,               // tail sequence length equals 0
    0                // unnecessary value, because tail is disabled
};
uint8_t RawData[4] = {0X3A,0X2B,0X5E,0X8F};
io_SendDataPackage(&cosio,RawData,4,&DefaultPkgFmt);
```

### 5.3 使用方法

#### 5.3.1 以指定格式解析数据包

以下样例是TF-Luna激光测距传感器驱动代码节选。

##### 创建数据包解析格式

```c
/* 创建一个报文头序列 */
const uint8_t TFLunaPkgHead[2]={0x59,0x59};

/* 创建一个数据包格式 */
const COS_PkgFmt TFLunaPkgFmt=
{
    1,               // enable package head
    TFLunaPkgHead,   // use default head sequence
    2,               // head sequence length equals two
    0,               // disable data check
    NULL,            // don't use check algorithm
    0,               // disable tail
    NULL,            // use null tail sequence
    0,               // tail sequence length equals 0
    0                // tail after check value. unnecessary value, because tail is disabled
};

/* 创建一个数据包回调函数 */
void TFLuna_ioPkgProccb(COS_io* ioDevice,uint8_t *DataPkg)
{
    ...
}
```

##### 将io设备设置为数据包模式

```c
io_PackageMode(&(NewTFLuna->ioDevice), 1, 7, TFLuna_ioPkgProccb);
```

##### 让io设备使用自定义解析格式

```c
io_SetPkgParseFmt(&(NewTFLuna->ioDevice),&TFLunaPkgFmt);
```

#### 5.3.2 无格式接收指定长度数据

##### 特殊配置数据包解析格式

需要把数据包头、尾、校验都删掉，并指定数据长度。
```c
const COS_PkgFmt TFLunaPkgFmt=
{
    0,               // disable package head
    NULL,            // unuse head sequence
    0,               // head sequence length equals 0
    0,               // disable data check
    NULL,            // don't use check algorithm
    0,               // disable tail
    NULL,            // use null tail sequence
    0,               // tail sequence length equals 0
    0                // tail after check value. unnecessary value, because tail is disabled
};

```

##### 加载解析格式

```c
io_PackageMode(&(NewTFLuna->ioDevice), 1, 7, TFLuna_ioPkgProccb);
io_SetPkgParseFmt(&(NewTFLuna->ioDevice),&TFLunaPkgFmt);
```

### 5.4 代码分析

#### 5.4.1 数据包格式结构体

```c
typedef struct PackageDefinition
{
	uint8_t  HeadEnable;
	const uint8_t *Head;
	uint8_t  HeadLength;

	uint8_t  CheckValEnable;
	uint16_t(*CheckAlgorithm)(uint8_t *,uint32_t);

	uint8_t  TailEnable;
	const uint8_t *Tail;
	uint8_t  TailLength;

	uint8_t  TailBehindChk;

}COS_PkgFmt;
```
**成员列表**

1. **HeadEnable**:启用数据包头
2. **Head**:数据包头序列
3. **HeadLength**:数据包头长度
4. **CheckValEnable**:启用校验值
5. **CheckAlgorithm**:校验算法，函数指针。
6. **TailEnable**:启用数据包尾
7. **Tail**:数据包尾序列
8. **TailLength**:数据包尾长度
9. **TailBehindChk**:若为1则表示数据包尾在校验值之后出现

**使用示例**

```c
const COS_PkgFmt DefaultPkgFmt=
{
    1,               // enable package head
    DefaultPkgHead,  // use default head sequence
    2,               // head sequence length equals two
    1,               // enable data check
    Chk_XOR,         // use XOR check algorithm
    0,               // disable tail
    NULL,            // use null tail sequence
    0,               // tail sequence length equals 0
    0                // unnecessary value, because tail is disabled
};
```

调用`io_SetPkgParseFmt(COS_io*,COS_PkgFmt*)`函数加载该格式至io设备。或者调用`io_SendDataPackage(COS_io*,void*,uint32_t,COS_PkgFmt*)`函数来按照上文创建的`DefaultPkgFmt`格式发送数据包。


#### 5.4.2 数据包解析过程

暂无，参照代码文件及其注释  `CarOS/src/communication/c_pkgproc.c`

## 6. 模块拆卸指导