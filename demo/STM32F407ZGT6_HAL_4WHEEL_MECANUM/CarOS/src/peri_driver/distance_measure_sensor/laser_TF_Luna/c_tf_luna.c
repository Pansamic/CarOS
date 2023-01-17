/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_tf_luna.c
Author:      Pansamic
Version:     0.1
Create date: 2023.1.14
Description: This file contains all supported peripherals.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.14     create v0.1 version.
*****************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif
#include "./src/memory/c_memory.h"
#include "./src/peri_driver/distance_measure_sensor/laser_tf_luna/c_tf_luna.h"

#define GetTFLunaFromio(addr,type,field) ((type*)((unsigned char*)addr - (unsigned long)&((type*)0)->field))
const uint8_t TFLunaPkgHead[2]={0x59,0x59};

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


void TFLuna_ioPkgProccb(COS_io* ioDevice,uint8_t *DataPkg);


TFLuna* AddPeri_TFLuna(const char *Name,COS_uart huart)
{
	TFLuna *NewTFLuna = (TFLuna*)cos_malloc(sizeof(TFLuna));
	uint8_t *InputBuf = (uint8_t*)cos_malloc(LUNA_IO_INPUT_BUF_LEN);
	uint8_t *OutputBuf1 = (uint8_t*)cos_malloc(LUNA_IO_OUTPUT_BUF1_LEN);
	uint8_t *OutputBuf2 = (uint8_t*)cos_malloc(LUNA_IO_OUTPUT_BUF2_LEN);

	io_Init(&(NewTFLuna->ioDevice), Name, huart, InputBuf, LUNA_IO_INPUT_BUF_LEN, OutputBuf1, LUNA_IO_OUTPUT_BUF1_LEN, OutputBuf2, LUNA_IO_OUTPUT_BUF2_LEN);
	io_PackageMode(&(NewTFLuna->ioDevice), 1, 7, TFLuna_ioPkgProccb);
	io_SetPkgParseFmt(&(NewTFLuna->ioDevice),&TFLunaPkgFmt);
	TFLuna_Reset(NewTFLuna);
	COS_printf("TF-Luna laser sensor '%s' is ready.\r\n",Name);
	return NewTFLuna;
}

void TFLuna_Reset(TFLuna *TFLuna_instance)
{
	static const uint8_t ResetSequence[5] = {0x5A,0x05,0x05,0x06,0x00};
	io_SendData(&(TFLuna_instance->ioDevice),ResetSequence,5);
	TFLuna_instance->Amplification = 0;
	TFLuna_instance->Distance = 0;
}

uint16_t TFLuna_GetDistance(TFLuna *TFLuna_instance)
{
	return TFLuna_instance->Distance;
}

uint16_t TFLuna_GetAmp(TFLuna *TFLuna_instance)
{
	return TFLuna_instance->Amplification;
}

void TFLuna_ioPkgProccb(COS_io* ioDevice,uint8_t *DataPkg)
{
	TFLuna* pTFLuna = GetTFLunaFromio(ioDevice,TFLuna,ioDevice);
	uint16_t Distance = DataPkg[0]|DataPkg[1]<<8;
	uint16_t Amplification = DataPkg[2]|DataPkg[3]<<8;
	if(Distance != 0 && Amplification > 100)
	{
		pTFLuna->Distance = DataPkg[0]|DataPkg[1]<<8;
	}
	else
	{
		__NOP();
	}
	pTFLuna->Amplification = Amplification;
}
#ifdef __cplusplus
}
#endif
