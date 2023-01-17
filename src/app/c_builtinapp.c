/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_biltinapp.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.2
Description: This file contains an application list and some definition of
    application callback functions.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.2    create v0.1 version.
*****************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif
#include "c_config.h"
#include "./src/debug/c_debug.h"
#include "./src/app/c_app.h"
#include "./src/app/c_builtinapp.h"
#include "./src/car/c_car.h"

#if LOG_STORAGE
void PrintLogcb           (uint8_t* ParamList);
#endif
void PackageModecb        (uint8_t *ParamList);
/* stream mode callback function */
void PackageTest          (COS_io* ioDevice, uint8_t* DataPackage);

COS_App BuiltinAppList[BUILT_IN_APP_AMOUNT]={
#if LOG_STORAGE
		/* print all system log */
		{"dmesg", NULL, PrintLogcb},

		{"pkg", NULL, PackageModecb}
#endif
};



#if LOG_STORAGE
void PrintLogcb(uint8_t* ParamList)
{
	COS_PrintLog();
}
#endif


void PackageModecb(uint8_t *ParamList)
{
	io_PackageMode(&cosio, 1, 4, PackageTest);
}


void PackageTest(COS_io* ioDevice, uint8_t* DataPackage)
{
	io_SendData(&cosio, DataPackage, 4);
	if(*DataPackage == '+')
	{
		io_CommandMode(&cosio);
	}
}









#ifdef __cplusplus
}
#endif
