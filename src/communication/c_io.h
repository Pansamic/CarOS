/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_io.h
Author:      Pansamic
Version:     1.0
Create date: 2023.1.15
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.15    create v1.0 version.
*****************************************************************************/
#ifndef __C_COMMUNICATION_H__
#define __C_COMMUNICATION_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdarg.h>
#include "./src/c_includes.h"
#include "./src/communication/c_command.h"
#include "./src/communication/c_pkgproc.h"
/**********************************************************************************************
 *                                                                                            *
 *                                         MACROS                                             *
 *                                                                                            *
 **********************************************************************************************/

#define COMMAND_MODE              0
#define PACKAGE_MODE              1

#define MAXIODEVICEAMOUNT         6

/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/

/**********************************************************************************************
 * @brief:This struct describes a general model of input/outpur device. Input buffer is a ring
 *     buffer. Output module uses ping-pong buffer technique.
 * @params:
 *     1.Name:A string containing the name of io device.
 *     2.huart:the pointer of usart that transmit and receive data.
 *     3.ActivatedOutBufIndex:current writable buffer index.
 *     4.Outputing:1 when buffer data is transmitting, 0 when no buffer is transmitting.
 *     5.OutputBuf:an array that contains two buffer address.
 *     6.OutputBufSize:an array that contains the size of both output buffers.
 *     7.OutputBufPosition:an array that contains the length of data that is put into buffer.
 *     8.InputBuf:the pointer of input buffer.
 *     9.InputBufSize:size of input buffer.
 *     10.InputHeadIndex:ring buffer head index.
 *     11.InputTailIndex:ring buffer tail index.
 *     12.InputBufFull:a flag indicating whether input buffer is full. its value can be 0 or 1.
 *     13.CmdList:chain of 'COS_Cmd' type node.
 *     14.DataPackage:if the value is 0, it means the io device works in command mode.
 *         if the value is greater than 0, that means the io device works in data package mode.
 * */
typedef struct ioDeviceDefinition
{
	const char *Name;
	COS_uart   huart;
	/* double buffer */
	uint8_t    ActivatedOutBufIndex;
	uint8_t    Outputing;
	uint8_t   *OutputBuf[2];
	uint32_t   OutputBufSize[2];
	uint32_t   OutputBufPosition[2];


	/* input buffer related */
	uint8_t   *InputBuf;
	uint32_t   InputBufSize;
	uint32_t   InputHeadIndex;
	uint32_t   InputTailIndex;
	uint8_t    InputBufFull;

	uint8_t    ReceiveMode;
	COS_Cmd    *CmdList;
	COS_PkgPrc *PkgProcessor;
}COS_io;

/**********************************************************************************************
 *                                                                                            *
 *                                         EXTERN                                             *
 *                                                                                            *
 **********************************************************************************************/
/* system built-in io device */
extern COS_io cosio;

/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/
/* main operation function */
void       io_Init                    (COS_io *ioDevice, const char* Name, COS_uart huart, uint8_t *InputBuf, uint32_t InputBufLen, uint8_t *OutputBuf1, uint32_t OutputBuf1Len, uint8_t *OutputBuf2, uint32_t OutputBuf2Len);
void       COS_printf                 (const char *fmt,...);
void       cosioInit                  ();
void       cosioTransmitOver          ();
void       io_printf                  (COS_io *ioDevice,const char *fmt,...);
void       io_Process                 ();
void       io_DMAHandler              (COS_uart huart);

/* output related */
void       io_SendData                (COS_io *ioDevice, void *pData, uint32_t Length);
void       io_OutputProcess           (COS_io *ioDevice);
void       io_vprintf                 (COS_io *ioDevice,const char *fmt, va_list ap);
uint32_t  _io_GetSpareOutBuf          (COS_io *ioDevice, uint8_t **pDataDst);
uint8_t   _io_ActivatedBufEmpty       (COS_io *ioDevice);
void      _io_Transmit                (COS_io *ioDevice, uint8_t *pData, uint32_t Length);

/* input related */
void       io_InputProcess            (COS_io *ioDevice);
void       io_UartRxIntHandler        (COS_uart huart);
void       io_InputBufWrite           (COS_io *ioDevice, uint8_t *pData, uint32_t Length);
void      _io_InputBufWriteByte       (COS_io *ioDevice, uint8_t Data);
uint32_t   io_GetLine                 (COS_io *ioDevice, uint8_t *pDataDst, uint32_t DstLength);
uint32_t   io_GetData                 (COS_io *ioDevice, uint8_t *pDataDst, uint32_t Length);
uint8_t   _io_GetByte                 (COS_io *ioDevice, uint8_t *pDataDst);
void       io_InputBufRemove          (COS_io *ioDevice, uint32_t Length);
uint8_t    io_InputBufRemoveByte      (COS_io *ioDevice);
void       io_InputBufReset           (COS_io *ioDevice);
uint8_t    io_InputBufFull            (COS_io *ioDevice);
uint8_t    io_InputBufEmpty           (COS_io *ioDevice);
uint32_t   io_InputBufCapacity        (COS_io *ioDevice);
uint32_t   io_InputBufGetSize         (COS_io *ioDevice);
uint32_t   io_InputBufGetSpare        (COS_io *ioDevice);

/* receive mode */

void       io_CommandMode             (COS_io *ioDevice);




#ifdef __cplusplus
}
#endif
#endif
