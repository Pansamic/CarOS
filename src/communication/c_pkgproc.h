/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_pkgproc.h
Author:      Pansamic
Version:     1.0
Create date: 2023.1.15
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.15    create v1.0 version.
*****************************************************************************/
#ifndef __C_PKGPROC_H__
#define __C_PKGPROC_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdarg.h>
#include "./src/c_includes.h"

/**********************************************************************************************
 *                                                                                            *
 *                                         MACROS                                             *
 *                                                                                            *
 **********************************************************************************************/
#define PKG_DEFAULT_HEAD1         0XFE
#define PKG_DEFAULT_HEAD2         0XEF


/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/

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
/**********************************************************************************************
 * @brief:
 * @params:
 *     1.DatapackageSize:
 *     2.DataDst:
 *     3.Datacb:
 */
typedef struct ioDataPackageProcessor
{
	uint8_t     FormatEnable;
	COS_PkgFmt *PackageFormat;
	uint32_t    RawDataSize;
	uint8_t    *DataDst;
	void      (*PkgProcesscb)(COS_io*,uint8_t*);
}COS_PkgPrc;


/**********************************************************************************************
 *                                                                                            *
 *                                         EXTERN                                             *
 *                                                                                            *
 **********************************************************************************************/

/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/
/* package mode related */
void       io_PackageMode             (COS_io *ioDevice, uint8_t FormatEnable, uint8_t RawDataSize, void *CallbackFunc);
uint8_t    io_PackageProcess          (COS_io *ioDevice);
void       io_PkgProcessorDeinit      (COS_io *ioDevice);
void       io_SetPkgParseFmt          (COS_io *ioDevice, COS_PkgFmt *PkgFmt);
void       io_SendDataPackage         (COS_io *ioDevice, void *pData, uint32_t Length, COS_PkgFmt *PkgFmt);
uint8_t   _io_GetRawData              (COS_io *ioDevice);
uint16_t   Chk_XOR                    (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_CCITT            (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_CCITT_FALSE      (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_XMODEM           (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_X25              (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_MODBUS           (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_IBM              (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_MAXIM            (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_USB              (uint8_t *pData, uint32_t DataLen);
uint16_t   Chk_CRC16_DNP              (uint8_t *pData, uint32_t DataLen);
#ifdef __cplusplus
}
#endif
#endif
