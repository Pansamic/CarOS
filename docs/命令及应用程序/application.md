# 命令及应用程序

## 目录

[1. 简介](#1-简介)</br>
[2. 应用程序组成结构](#2-应用程序组成结构)</br>
----[2.1 应用程序名](#21-应用程序名)</br>
----[2.2 应用程序参数](#22-应用程序参数)</br>
----[2.3 回调函数](#23-回调函数)</br>
--------[2.3.1 格式](#231-格式)</br>
--------[2.3.2 函数内容](#232-函数内容)</br>
[3. 命令的组成结构](#3-命令的组成结构)</br>
[4. 应用程序实现方法](#4-应用程序实现方法)</br>
----[4.1 生成应用程序描述结构体](#41-生成应用程序描述结构体)</br>
----[4.2 挂载应用程序至io设备](#42-挂载应用程序至io设备)</br>
----[4.3 命令解释器识别应用程序](#43-命令解释器识别应用程序)</br>
----[4.4 命令解释器调用应用程序函数](#44-命令解释器调用应用程序函数)</br>

## 1. 简介

CarOS的应用程序仿照linux系统上的应用程序来制作。Windows系统中的应用程序一般都有图形化界面，然而有一些应用程序是基于命令行来使用的。CarOS希望构建一个串口命令行控制台（也就是io设备）来调用应用程序。CarOS简化了应用程序的概念为一个带有参数的函数，也没有系统命令行应用程序那样的可选参数，CarOS的应用程序的参数一开始设定好之后就不能修改的。

## 2. 应用程序组成结构

```c
typedef struct AppDefinition
{
    const char  *Name;
    const char  *ParamsDescription;
    void       (*CmdCallbackFunc)(uint8_t*);
}COS_App;
```

应用程序由三个部分组成：应用程序名、应用程序参数和应用程序回调函数。

### 2.1 应用程序名

应用程序名应该是一个字符串且尽量简短，如："scv"意为"set car velocity"。名称尽量简短是为了命令解释器解析得快一点。不过为了方便记忆也可以给你的应用程序起一个方便记忆的名称，虽然命令解释器会花稍微多一点时间来解析不过这个时间对整个系统的影响还是很小的。

### 2.2 应用程序参数

在COS_App结构体中，应用程序参数是用字符串来描述的，如：“int float uint32_t short”就表示这个应用程序的回调函数需要四个参数，其数据类型按顺序分别是int、float、uint32_t、short。

### 2.3 回调函数

#### 2.3.1 格式

回调函数的类型是`void (*)(uint8_t*)`，表示这个函数没有返回值、函数参数是uint8_t类型的指针。实际上这个指针是从io设备获取到的命令字符串中解析出来的实际参数值按顺序组成的一个小数据包，实际在调用这个函数时会把`COS_Cmd`结构体的成员`ParamsContainer`传给这个回调函数。
函数名称应该为`XXXXXcb`，以cb（callback-回调）结尾。函数声明应该是这样的：
```c
void XXXXXcb(uint8_t* ParamList);
```
建议函数的参数名称写为`ParamList`。不是不能写成别的，而是系统内置应用程序也都这么写。统一一点比较好看。

#### 2.3.2 函数内容

你可能比较好奇的是：明明我参数描述里面写了好几个参数，比如`“float float float”`，为什么回调函数的参数永远都是只有一个`uint8_t*`类型的指针？不能把函数直接定义为`void (*)(float,float,float)`类型的吗？还真不行，因为我要告诉`COS_Cmd`结构体，在它从输入缓冲区解析出参数值之后该调用哪个函数，就必须以回调函数的形式，在这结构体初始化的时候就把函数指针赋值给它，而如果要把函数指针赋值给结构体成员的话，函数的参数类型也必须与结构体成员的定义相同，那么这就决定了应用程序的所有回调函数都必须统一参数才能把函数指针赋值给结构体。那我们把参数值储存在一块区域里面，然后按照一定的格式读取出来就行了，这也就是为什么应用程序的所有回调函数的参数类型都必须是那块存储参数值的内存的指针。
至于为什么是`uint8_t*`，这是因为在RAM中最小存储单位是byte-字节，一字节是8bit，uint8_t就是纯纯8bit数据也就是一字节。float类型占4字节、short2字节、int4字节等等，我们按照这些数据类型的大小挨个在参数存储内存区中读取参数值就行了。

以`"float uint8_t int"`描述的参数列表为例，回调函数模板应该这么写：
```c
void XXXXXcb(uint8_t *ParamList)
{
    float Param1 = 0;
    uint8_t Param2 = 0;
    int Param3 = 0;
    uint16_t SizeSum = 0;

    Param1 = *(float*)(ParamList+SizeSum);
    SizeSum += sizeof(float);
    Param2 = *(uint8_t*)(ParamList+SizeSum);
    SizeSum += sizeof(uint8_t);
    Param3 = *(int*)(ParamList+SizeSum);

    /* 至此获取到了三个预设类型的参数值 */
    ...
}
```
有高手可能要问为什么不用va_list类型来包装一下获取参数的过程。如果您有更便捷、更安全、更直观的方式读取参数值，请务必告诉作者：pansamic@foxmail.com，不尽感激！



## 3. 命令的组成结构

```c
typedef struct ioCmdDefinition
{
	const char  *CmdName;
	const char  *ParamsDescription;
	uint8_t     *ParamsContainer;
	void       (*CmdCallbackFunc)(uint8_t*);
	struct ioCmdDefinition *pNext;
}COS_Cmd;
```

命令结构体由5个成员组成。
1. **CmdName**:命令名称，也就是应用程序名称，字符串。
2. **ParamsDescription**:用于描述参数类型的字符串，每个类型用空格隔开，如："int float uint32_t short"。
3. **ParamsContainer**:用于保存这个命令参数值的缓存区，其大小由成员'ParamsDescription'决定，在初始化命令时被动态分配内存。
4. **CmdCallbackFunc**:命令回调函数指针。
5. **pNext**:链表钩子。

## 4. 应用程序实现方法

### 4.1 生成应用程序描述结构体

```c
void SetVelocitycb        (uint8_t* ParamList);

COS_App AppList[1]={
    /* set motor velocity */
    {"smv", "float,float,float,float", SetVelocitycb},
};
void SetVelocitycb(uint8_t* ParamList)
{
    /* 获取参数值 */
    uint8_t SizeSum = 0;
    float LeftFrontVelocity = *(float*)(ParamList + SizeSum);
    SizeSum += sizeof(float);
    float LeftRearVelocity = *(float*)(ParamList + SizeSum);
    SizeSum += sizeof(float);
    float RightFrontVelocity = *(float*)(ParamList + SizeSum);
    SizeSum += sizeof(float);
    float RightRearVelocity = *(float*)(ParamList + SizeSum);
    ...
}
```

### 4.2 挂载应用程序至io设备

```c
LoadApp(&XXXio, AppList, 1);
```

该函数按照AppList中指定的内容生成并初始化若干个`COS_Cmd`结构体然后以链表的形式挂载在`COS_io`结构体的`COS_Cmd *CmdList`指针上。

### 4.3 命令解释器识别应用程序

其他设备通过串口输入数据后，命令解释器会从输入缓冲区中读取一行命令（实际上是一个字符串）然后识别出位于字符串开头的应用程序名。程序会遍历挂载在`COS_io`结构体的`COS_Cmd *CmdList`链表，看看有没有名称匹配的应用程序。

### 4.4 命令解释器调用应用程序函数

如果有，就继续解析字符串中的参数然后把参数值存储在`COS_Cmd`的`ParamsContainer`这个存储区域中。然后把这个存储区域的指针作为参数传递给回调函数并执行回调函数。
