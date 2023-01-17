/*****************************************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_memory.h
Author:      Pansamic
Version:     0.3
Create date: 2022.12.20
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2022.12.20    create v0.1 version.
*****************************************************************************/
#ifndef __MEMORY_H
#define __MEMORY_H
#include "./src/c_includes.h"
#ifndef NULL
#define NULL 0
#endif



//内存管理控制器
struct _m_mallco_dev
{
    void    (*init)(void);            //初始化
    uint8_t (*perused)(void);         //内存使用率
    uint8_t  *membasex;               //内存池，管理2个区域的内存
    uint16_t *memmapx;                //内存管理状态表
    uint8_t   memrdyx;                //内存管理是否就绪
};
extern struct _m_mallco_dev mallco_dev;                //在mallco.c里面定义 ;;这里进行外部声明；



void      Mem_Init        (void);                      //内存管理初始化函数(外/内部调用)
uint8_t   Mem_GetUsedRate (void);                      //获得内存使用率(外/内部调用)
void      cos_free        (void *ptr);                 //内存释放(外部调用)
void     *cos_malloc      (uint32_t size);             //内存分配(外部调用)
void     *cos_realloc     (void *ptr,uint32_t size);   //重新分配内存(外部调用)
#endif
