/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_tf_luna.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.14
Description: This file contains all supported peripherals.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.14     create v0.1 version.
*****************************************************************************/
#ifndef __PERI_DRIVER_H__
#define __PERI_DRIVER_H__
#ifdef __cplusplus
extern "C"{
#endif
#include "./src/c_includes.h"
#include "./src/communication/c_io.h"

#define LUNA_IO_INPUT_BUF_LEN 1024
#define LUNA_IO_OUTPUT_BUF1_LEN 128
#define LUNA_IO_OUTPUT_BUF2_LEN 64

typedef struct
{
	uint16_t Distance;
	uint16_t Amplification;
	COS_io   ioDevice;
}TFLuna;


TFLuna*  AddPeri_TFLuna     (const char *Name,COS_uart huart);
void     TFLuna_Reset       (TFLuna *TFLuna_instance);
uint16_t TFLuna_GetDistance (TFLuna *TFLuna_instance);
uint16_t TFLuna_GetAmp      (TFLuna *TFLuna_instance);

#ifdef __cplusplus
}
#endif
#endif
