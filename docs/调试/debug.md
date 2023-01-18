# 调试模块用户手册

## 1.简介

整个调试系统包括日志系统、断言机制

## 2.日志系统

想做一个日志记录器，能够记录下系统发生的事件，这样方便用户调试小车，免得用户在调车的时候半天不知道自己小车到底出了什么问题。

### 2.1 日志记录等级

0. emerg:紧急情况，系统不可用，系统面临崩溃
1. alert:警报，需要立即修复
2. crit:危险情况，可能会阻碍程序功能
3. err:一般错误信息，有损坏系统功能的潜在危险
4. warning:一般警告信息，可能会生成错误
5. notice:不是警告或者错误，但仍需要注意
6. info:一般性信息
7. debug:调试信息

### 2.2 系统日志配置选项

在`CarOS/c_debug.h`文件中有关于日志的配置

```c
/* COS system debug Enable */
#define COS_DEBUG 1

/* COS components debug Enable */
#if COS_DEBUG==1

#define DEBUG_CAR     1
#define DEBUG_DCMOTOR 1
#define DEBUG_ENCODER 1

/* Motor debug configuration */
#if DEBUG_DCMOTOR==1
    #define DEBUG_L298N   1
    #define DEBUG_A4950   1
#endif

/* if this macro equals to 1, CarOS system will print
 * log information realtime */
#define LOG_PRINT         1

/* if this macro equals 1, CarOS system will store
 * log information in RAM or other storage meterials */
#define LOG_STORAGE       1

/* Log level enable */
#define LOG_EMERG         1
#define LOG_ALERT         1
#define LOG_CRIT          1
#define LOG_ERR           1
#define LOG_WARNING       1
#define LOG_NOTICE        1
#define LOG_INFO          1
#define LOG_DEBUG         0   // debug mode, records almost everything, but occupies much memory and MCU processing speed

#if LOG_STORAGE
#define LOG_MEM_SIZE      10  // no lower than 2KB is recommended. unit:kb
#endif
```

`COS_DEBUG`是全局日志系统使能，当不需要日志系统记录时可以选择性关闭日志系统以提升整体性能。

`LOG_XXX`是各等级日志记录选项，值为1时使能，值为0时关闭。

`LOG_PRINT`是实时打印日志的选项，为1时实时打印信息至串口控制台

`LOG_STORAGE`为1时存储日志，能够使用`COS_PrintLog()`，若为0则无法使用`COS_PrintLog()`函数来打印日志信息。

### 2.3 日志记录接口

#### :round_pushpin: 日志记录
```c
void COS_Logf(LogLevel,fmt,...);
```
**简介**

该函数使用格式化字符串记录当前文件、函数名、行数的调试信息并储存在某处。

**参数列表**
1. LogLevel:日志等级
2. fmt:格式化字符串
3. ...:格式化字符串参数值

**使用示例**
```c
COS_Logf(err,"'%s' read error:no input buffer","left sensor");
```

### 2.4 日志打印接口

```c
void COS_PrintLog();
```
**简介**

该函数遍历所有日志节点生成日志文本，并通过串口控制台输出。

**使用示例**

```c
COS_PrintLog();
```


### 2.5 日志系统代码架构

#### 调用日志记录函数

调用`COS_Logf()`函数后，该函数会被宏定义识别成另外一个函数。实际上，`COS_Logf()`的原型是：
```c
#define COS_Logf(level,format, arg...) _Log_Write( __FILE__,__FUNCTION__, __LINE__, level, format, ##arg)
```

#### 日志过滤器

调用`COS_Logf()`后，函数首先会根据`c_config.h`中的配置选项过滤一下该条日志，如果该日志不应该被记录，那么直接退出函数。日志过滤的配置项如下，都是`c_config.h`文件中的配置项：
```c
#define DEBUG_CAR     1
#define DEBUG_DCMOTOR 1
#define DEBUG_ENCODER 1

/* Motor debug configuration */
#if DEBUG_DCMOTOR==1
    #define DEBUG_L298N   1
    #define DEBUG_A4950   1
#endif
/* Log level enable */
#define LOG_EMERG         1
#define LOG_ALERT         1
#define LOG_CRIT          1
#define LOG_ERR           1
#define LOG_WARNING       1
#define LOG_NOTICE        1
#define LOG_INFO          1
#define LOG_DEBUG         0   // debug mode, records almost 
```


#### 日志记录函数做了什么

在`_Log_Write()`函数中，它生成一个`Log_Node`类型的结构体。
```c
typedef struct LogNodeDefinition
{
    uint8_t LogLevel;                // log level, ranging from 0 to 7
    const char *FileName;           // the name of file where log macro resides
    const char *FunctionName;       // the name of the function where log macro resides
    uint16_t RowNumber;             // row number of log macro
    char *LogString;               // log information string
    int LogStringLen;              // LogStringLen locates the string
    struct LogNodeDefinition *Next;// next log chain node

}LogNode;
```
这个结构体称为日志节点，它储存了日志的信息，以链表的形式挂载在全局变量`LogNode *LogChainHead`上。如果`c_config.h`文件中打开了`LOG_PRINT`宏定义，那么`_Log_Write()`函数不仅会记录日志，还会同步打印日志。

该函数还把格式化后的字符串内容复制到全局变量`char COS_Log[1024*LOG_MEM_SIZE]`中，以便后续调用`COS_PrintLog()`输出所有日志。
