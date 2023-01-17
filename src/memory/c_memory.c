/*****************************************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_memory.c
Author:      Pansamic
Version:     0.3
Create date: 2022.12.20
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2022.12.20    create v0.1 version.
*****************************************************************************/
#include "./src/memory/c_memory.h"
 
/*****************************************************************************************
 *                                                                                       *
 *                                      MACROS                                           *
 *                                                                                       *
 *****************************************************************************************/
/*Memory pool 4-byte alignment; __align(4) defines the memory
 * pool to be 4-byte aligned. This is very important! Without
 * this restriction, in some cases [such as allocating memory
 * to structure Pointers], an error may occur, so be sure to
 * add this*/
uint8_t MemBase[MEM_MAX_SIZE*1024] __attribute__ ((aligned (4)));   // MCU embeded SRAM

/* memory management table */
uint16_t MemMapBase[MEM_ALLOC_TABLE_SIZE];      // inner SRAM pool table

/* memory management parameters */
const uint32_t MemTableSize=MEM_ALLOC_TABLE_SIZE; // memory table size
const uint32_t MemBlockSize=MEM_BLOCK_SIZE;       // memory block size
const uint32_t MemSize=MEM_MAX_SIZE*1024;              // memory maximum size


uint32_t  __malloc  (uint32_t size);    //内存分配(内部调用)
uint8_t   __free    (uint32_t offset);  //内存释放(内部调用)
void      __memset(void *s,uint8_t c,uint32_t count);  //设置内存
void      __memcpy(void *des,void *src,uint32_t n);    //复制内存

//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
	Mem_Init,     //内存初始化
    Mem_GetUsedRate,  //内存使用率
    MemBase,      //内存池
    MemMapBase,   //内存管理状态表
    0,            //内存管理未就绪
};

//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void __memcpy(void *des,void *src,uint32_t n)
{ 
    uint8_t *xdes=des;
    uint8_t *xsrc=src;
    while(n--)*xdes++=*xsrc++; 
} 
//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
void __memset(void *s,uint8_t c,uint32_t count)
{ 
    uint8_t *xs = s;
    while(count--)*xs++=c; 
}   
//内存管理初始化 
//memx:所属内存块
void Mem_Init()
{ 
    __memset(mallco_dev.memmapx , 0,MemTableSize*2);   //内存状态表数据清零
    __memset(mallco_dev.membasex, 0,MemSize);           //内存池所有数据清零
    mallco_dev.memrdyx=1;                               //内存管理初始化OK
} 
//获取内存使用率
//返回值:使用率(0~100)
uint8_t Mem_GetUsedRate(void)
{ 
    uint32_t used=0;
    uint32_t i;
    for(i=0;i<MemTableSize;i++)
    { 
        if(mallco_dev.memmapx[i])used++;
    }
    return (used*100)/(MemTableSize);
} 
//内存分配(内部调用)
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址
uint32_t __malloc(uint32_t size)
{ 
    signed long offset=0; 
    uint16_t nmemb; //需要的内存块数
    uint16_t cmemb=0;//连续空内存块数
    uint32_t i;
    if(!mallco_dev.memrdyx)mallco_dev.init();//未初始化,先执行初始化
    if(size==0)return 0XFFFFFFFF;            //不需要分配

    nmemb=size/MemBlockSize;                //获取需要分配的连续内存块数
    if(size%MemBlockSize)nmemb++;
    for(offset=MemTableSize-1;offset>=0;offset--)     //搜索整个内存控制区
    {    
		if(!mallco_dev.memmapx[offset])cmemb++;     //连续空内存块数增加
		else cmemb=0;        //连续内存块清零
		if(cmemb==nmemb)       //找到了连续nmemb个空内存块
		{
			for(i=0;i<nmemb;i++)       //标注内存块非空
			{
				mallco_dev.memmapx[offset+i]=nmemb;
			}
            return (offset*MemBlockSize);             //内存块的大小*偏移量=返回偏移地址
		}
    }
    return 0XFFFFFFFF;//未找到符合分配条件的内存块 
} 
//释放内存(内部调用)
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败; 
uint8_t __free(uint32_t offset)
{ 
    int i; 
    if(!mallco_dev.memrdyx)//未初始化,先执行初始化
 {
  mallco_dev.init();   
        return 1;//未初始化 
    } 
    if(offset<MemSize)                              //偏移必须发生在内存池内.
    { 
        int index=offset/MemBlockSize;       //偏移所在内存块号码
        int nmemb=mallco_dev.memmapx[index];     //内存块数量
        for(i=0;i<nmemb;i++)        //内存块清零
        { 
            mallco_dev.memmapx[index+i]=0; 
        } 
        return 0; 
    }
    else
    {
    	return 2;//偏移超区了.
    }
} 
//释放内存(外部调用)
//ptr:内存首地址
void cos_free(void *ptr)
{ 
    uint32_t offset;
    if(ptr==NULL)return;  //地址为0. 
    offset=(uint32_t)ptr-(uint32_t)mallco_dev.membasex;
    __free(offset);    //释放内存
} 
//分配内存(外部调用)
//size:内存大小(字节)
//返回值:分配到的内存首地址.
void *cos_malloc(uint32_t size)
{ 
	uint32_t offset;
	offset=__malloc(size);
    if(offset==0XFFFFFFFF)return NULL; 
    else
    {
    	return (void*)((uint32_t)mallco_dev.membasex+offset);
    }
} 
//重新分配内存(外部调用)
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
void *cos_realloc(void *ptr,uint32_t size)
{ 
    uint32_t offset;
    offset=__malloc(size);
    if(offset==0XFFFFFFFF)
    {
    	return NULL;
    }
    else 
    {             
        __memcpy((void*)((uint32_t)mallco_dev.membasex+offset),ptr,size);     //拷贝旧内存内容到新内存
        cos_free(ptr);                     //释放旧内存
        return (void*)((uint32_t)mallco_dev.membasex+offset);          //返回新内存首地址
    } 
} 
