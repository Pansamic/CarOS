/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_command.h
Author:      Pansamic
Version:     1.0
Create date: 2023.1.15
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.15    create v1.0 version.
*****************************************************************************/
#ifndef __C_COMMAND_H__
#define __C_COMMAND_H__
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
#define PARAMTYPE_char            1
#define PARAMTYPE_unsigned_char   2
#define PARAMTYPE_int             3
#define PARAMTYPE_unsigned_int    4
#define PARAMTYPE_short           5
#define PARAMTYPE_unsigned_short  6
#define PARAMTYPE_long            7
#define PARAMTYPE_unsigned_long   8
#define PARAMTYPE_float           9
#define PARAMTYPE_double          10
#define PARAMTYPE_int8_t          11
#define PARAMTYPE_int16_t         12
#define PARAMTYPE_int32_t         13
#define PARAMTYPE_uint8_t         14
#define PARAMTYPE_uint16_t        15
#define PARAMTYPE_uint32_t        16

/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/
typedef struct ioDeviceDefinition COS_io;

/**********************************************************************************************
 * @brief:
 * @params:
 *    1.CmdName:a string of command name.
 *    2.ParamsDescription:a string that describes parameter of the command. e.g."float,uint8_t"
 *    3.ParamsContainer:a buffer that contains the value of command. Its capacity is
 *        initialized as the sum of the spaces taken up by all the parameters.
 *    4.CmdCallbackFunc:the function that will be called when the command is input.
 *    5.pNext:next pointer of 'COS_Cmd'.
 * */
typedef struct ioCmdDefinition
{
	const char  *CmdName;
	const char  *ParamsDescription;
	uint8_t     *ParamsContainer;
	void       (*CmdCallbackFunc)(uint8_t*);
	struct ioCmdDefinition *pNext;
}COS_Cmd;

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
/* command related */
void       io_CommandMode             (COS_io *ioDevice);
void       io_AddCmd                  (COS_io *ioDevice, const char *CmdName, const char *ParamsDescription, void* CmdCallbackFunc);

void      _io_CmdProcess              (COS_Cmd *CmdList, char *CmdStr);
int       _io_ScanWriteParamData      (char *str, uint8_t ParamType, uint8_t *pDataDst);
void      _io_CmdExecute              (COS_Cmd *Cmd);
uint8_t   _io_GetParamSizeByStr       (char *ParamStr);
uint8_t   _io_GetParamType            (char *Str);
uint8_t   _io_GetParamSizeByTypeCode  (uint8_t ParamType);

#ifdef __cplusplus
}
#endif
#endif
