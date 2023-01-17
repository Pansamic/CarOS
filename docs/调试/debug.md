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
7. debug:

### 2.2 系统日志配置选项

在`CarOS/c_debug.h`文件中有关于日志的配置

```c
#define LOG_ENABLE        1
#if (LOG_ENABLE==1)
#define LOG_EMERG         1
#define LOG_ALERT         1
#define LOG_CRIT          1
#define LOG_ERR           1
#define LOG_WARNING       0
#define LOG_NOTICE        0
#define LOG_INFO          0
#define LOG_DEBUG         0   // debug mode, records almost everything, but occupies much memory and MCU processing speed
#endif
```

`LOG_ENABLE`是全局日志系统使能，当不需要日志系统记录时可以选择性关闭日志系统以提升整体性能。

`LOG_XXX`是各等级日志记录选项，值为1时使能，值为0时关闭。

### 2.3 日志记录接口

### 2.4 日志打印接口

### 2.5 日志系统代码架构


