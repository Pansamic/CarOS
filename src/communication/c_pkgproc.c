/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_pkgproc.c
Author:      Pansamic
Version:     1.0
Create date: 2023.1.15
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.15    create v1.0 version.
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "./src/memory/c_memory.h"
#include "./src/debug/c_debug.h"
#include "./src/communication/c_io.h"

const uint8_t DefaultPkgHead[2]={PKG_DEFAULT_HEAD1,PKG_DEFAULT_HEAD2};

/*
 * @brief:Default data package format, loaded when calls 'io_PackageMode()' function.
 *     If you want to use customized package format, please call 'io_SetPkgParseFmt()'
 * */
const COS_PkgFmt DefaultPkgFmt=
{
    1,               // enable package head
    DefaultPkgHead,  // use default head sequence
    2,               // head sequence length equals two
    1,               // enable data check
    Chk_XOR,         // use XOR check algorithm
    0,               // disable tail
    NULL,            // use null tail sequence
    0,               // tail sequence length equals 0
    0                // unnecessary value, because tail is disabled
};

uint8_t  ExamPkgFmt             (COS_PkgFmt *PkgFmt);

/* Auxiliary functions of check algorithm functions */
void     InvertUint8            (uint8_t *dBuf, uint8_t *srcBuf);
void     InvertUint16           (uint16_t *dBuf,uint16_t *srcBuf);
void     InvertUint32           (uint32_t *dBuf,uint32_t *srcBuf);


/*****************************************************************************************************
 * @name:io_PackageMode
 * @brief:start io device receive stream mode, which enables io device to tramsmit binary data directly
 * @params:
 *     1.ioDevice:pointer of io device
 *     2.FormatEnable:decide whether use data package format to parse input data.If disable format,
 *         the input data will be directly fetched from input buffer. If enable, the package parse
 *         processor will find data package with specific format from input buffer and extract raw
 *         data from data package.
 *     2.RawDataSize:raw data in a data package size to be read at a time.
 *     3.CallbackFunc:pointer of callback funtion that process the received data package.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_PackageMode(COS_io *ioDevice, uint8_t FormatEnable, uint8_t RawDataSize, void *CallbackFunc)
{
	COS_PkgPrc *NewPkgPrc = (COS_PkgPrc*)cos_malloc(sizeof(COS_PkgPrc));
	uint8_t *DataDst = NULL;

	if(NewPkgPrc==NULL)
	{
		COS_Logf(err,"start package mode failed:memory not enough.");
		return;
	}
	ioDevice->PkgProcessor = NewPkgPrc;

	DataDst = (uint8_t*)cos_malloc(RawDataSize);

	if(DataDst==NULL)
	{
		COS_Logf(err,"start package mode failed:memory not enough.");
		cos_free(NewPkgPrc);
		return;
	}

	NewPkgPrc->FormatEnable = FormatEnable;
	NewPkgPrc->DataDst = DataDst;
	NewPkgPrc->RawDataSize = RawDataSize;
	NewPkgPrc->PkgProcesscb = CallbackFunc;

	if(FormatEnable==1)
	{
		/* Default Parse Protocol:
		 * 1.Enable package head, head bytes are 0xFE and 0xEF;
		 * 2.Enable package check byte, check algorithm is XOR;
		 * 3.Disable package tail, use package length to indentity the
		 * length of data package. */
		io_SetPkgParseFmt(ioDevice,&DefaultPkgFmt);
	}
	/* set receive mode as stream mode */
	ioDevice->ReceiveMode = PACKAGE_MODE;
}
/*****************************************************************************************************
 * @name:_io_PkgProcessorDeinit
 * @brief:
 * @params:
 *     1.ioDevice:pointer of io device
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void _io_PkgProcessorDeinit(COS_io *ioDevice)
{
	cos_free(ioDevice->PkgProcessor->DataDst);
	cos_free(ioDevice->PkgProcessor->PackageFormat);
	cos_free(ioDevice->PkgProcessor);
}
/*****************************************************************************************************
 * @name:io_SetPkgParseFmt
 * @brief:Sets the io device to receive data package and parse the package in the specified format.
 * @params:
 *     1.ioDevice:pointer of io device
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_SetPkgParseFmt(COS_io *ioDevice, COS_PkgFmt *PkgFmt)
{
	if(ioDevice->PkgProcessor==NULL)
	{
		COS_Logf(err,"'%s' set package parse error:Not in package mode.",ioDevice->Name);
		return;
	}
	if(!ExamPkgFmt(PkgFmt))
	{
		COS_Logf(err,"'%s' set package parse error:package format incorrect.",ioDevice->Name);
	}
	ioDevice->PkgProcessor->PackageFormat = PkgFmt;
	ioDevice->PkgProcessor->FormatEnable = 1;
}
/*****************************************************************************************************
 * @name:_io_PackageProcess
 * @brief:get raw data from input buffer and call stream callback function.
 * @params:
 *     1.ioDevice:pointer of io device
 * @retval: 1 if process secessfully ; 0 if process failed.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_PackageProcess(COS_io *ioDevice)
{
	int ErrorCode = 0;

	/* basic check */
	if(ioDevice->PkgProcessor==NULL)
	{
		COS_Logf(err,"io device %s work in receive stream mode but not initialized correctly.");
		return 0;
	}

	if(!ExamPkgFmt(ioDevice->PkgProcessor->PackageFormat))
	{
		COS_Logf(err,"io device '%s' parsing package error:package format illegal.",ioDevice->Name);
		return 0;
	}

	ErrorCode = _io_GetRawData(ioDevice);

	if(ErrorCode==1)
	{
		ioDevice->PkgProcessor->PkgProcesscb(ioDevice,ioDevice->PkgProcessor->DataDst);
		return 1;
	}
	else
	{
		return 0;
	}
}
/*****************************************************************************************************
 * @name:io_SendDataPackage
 * @brief:send a data package with a restriction of a specified package format.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pData:pointer of raw data.
 *     3.Length:length of raw data.
 *     4.PkgFmt:pointer of data package format struct--'COS_PkgFmt'.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_SendDataPackage(COS_io *ioDevice, void *pData, uint32_t Length, COS_PkgFmt *PkgFmt)
{
	uint16_t icv = 0;
	uint8_t *pFmtData = (uint8_t*)pData;

	if(!ExamPkgFmt(PkgFmt))
	{
		COS_Logf(err,"send data package error:data package format is not correct, please check the package format.");
		return ;
	}

	if(PkgFmt->HeadEnable)
	{
		io_SendData(ioDevice, PkgFmt->Head, PkgFmt->HeadLength);
	}
	io_SendData(ioDevice, pFmtData, Length);

	/* There are 4 combination of sequences between check value and tail */

	/* 1 *//* only check value */
	if(PkgFmt->CheckValEnable&&!PkgFmt->TailEnable)
	{
		/* generate check value */
		icv = PkgFmt->CheckAlgorithm(pData,Length);
		if(PkgFmt->CheckAlgorithm==Chk_XOR)
			io_SendData(ioDevice, &icv, 1);
		else
			io_SendData(ioDevice, &icv, 2);
	}

	/* 2 *//* only tail */
	if(!PkgFmt->CheckValEnable&&PkgFmt->TailEnable)
	{
		io_SendData(ioDevice, PkgFmt->Tail, PkgFmt->TailLength);
	}

	/* 3 *//* both tail and check value, but check value first */
	if(PkgFmt->CheckValEnable&&PkgFmt->TailEnable&&PkgFmt->TailBehindChk)
	{
		/* generate check value */
		icv = PkgFmt->CheckAlgorithm(pData,Length);
		if(PkgFmt->CheckAlgorithm==Chk_XOR)
			io_SendData(ioDevice, &icv, 1);
		else
			io_SendData(ioDevice, &icv, 2);
		io_SendData(ioDevice, PkgFmt->Tail, PkgFmt->TailLength);
	}

	/* 4 *//* both tail and check value, but tail first */
	if(PkgFmt->CheckValEnable&&PkgFmt->TailEnable&&!PkgFmt->TailBehindChk)
	{
		/* generate check value */
		icv = PkgFmt->CheckAlgorithm(pData,Length);
		io_SendData(ioDevice, PkgFmt->Tail, PkgFmt->TailLength);
		if(PkgFmt->CheckAlgorithm==Chk_XOR)
			io_SendData(ioDevice, &icv, 1);
		else
			io_SendData(ioDevice, &icv, 2);
	}




}

/*****************************************************************************************************
 * @name:_io_GetRawData
 * @brief:get data package from input buffer with package format.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: 1 if get raw data successfully ; 0 if failed to get raw data.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_GetRawData(COS_io *ioDevice)
{
    uint32_t RawDataSize = ioDevice->PkgProcessor->RawDataSize;
    /* useless data length from the beginning of input buffer
     * and will be delete after raw data is extracted */
	uint32_t UselessDataLength = 0;
	/* package beginning index in input buffer. */
	uint32_t PkgBgInInputBuf = ioDevice->InputTailIndex;
	/* package end index in input buffer */
	uint32_t PkgEdInInputBuf = 0;
	/* size of complete package, including head, tail and check value */
	uint32_t PkgSize = 0;
	uint32_t ActualRawLen = 0;
	uint8_t ChkValSize = ioDevice->PkgProcessor->PackageFormat->CheckValEnable ? ioDevice->PkgProcessor->PackageFormat->CheckAlgorithm==Chk_XOR ? 1:2 :0;
	/* if raw data size is 0 which means raw data length is not fixed,
	 * value of this variable is just the length of format, not including
	 * raw data length. */
	PkgSize = ioDevice->PkgProcessor->RawDataSize+
			  ioDevice->PkgProcessor->PackageFormat->HeadEnable ? ioDevice->PkgProcessor->PackageFormat->HeadLength : 0+
              ioDevice->PkgProcessor->PackageFormat->TailEnable ? ioDevice->PkgProcessor->PackageFormat->TailLength : 0+
              ChkValSize;
	if(_io_InputBufGetSize(ioDevice)<PkgSize)
	{
		return 0;
	}
	if(ioDevice->PkgProcessor->RawDataSize==0 && ioDevice->PkgProcessor->PackageFormat->TailEnable==0)
	{
		COS_Logf(err,"io device find data package error:both raw data size and tail aren't specified.",ioDevice->Name);
		return 0;
	}

                               /****************************/
                               /*  FIND PACKAGE BEGINNING  */
                               /****************************/
	/*-------------------------*/
	/* Sequence Matching Start */
	/*-------------------------*/
	/* find package start signal with brute-force algorithm */
	uint8_t TempHeadIndex = 0;
	/* find end byte regardless whether package head is specified or not */
	while(PkgBgInInputBuf != ioDevice->InputHeadIndex && TempHeadIndex<ioDevice->PkgProcessor->PackageFormat->HeadLength)
	{
		/* if data matches, continue finding */
		if(ioDevice->InputBuf[PkgBgInInputBuf]==ioDevice->PkgProcessor->PackageFormat->Head[TempHeadIndex])
		{
			/* The input buffer and header sequence match the next byte in turn */
			PkgBgInInputBuf = (PkgBgInInputBuf+1)%ioDevice->InputBufSize;
			TempHeadIndex++;
		}
		else // restart match
		{
			/* input buffer starts matching from next byte */
			PkgBgInInputBuf = (PkgBgInInputBuf-TempHeadIndex+1)%ioDevice->InputBufSize;

			/* Head sequence starts matching from beginning */
			TempHeadIndex=0;

			UselessDataLength++;
		}
	}
	/* if failed to find a header sequence */
	if(TempHeadIndex<ioDevice->PkgProcessor->PackageFormat->HeadLength)
	{
		/* remove useless data */
		_io_InputBufRemove(ioDevice, UselessDataLength);
		return 0;
	}
	/* else head sequence found successful */
	else
	{
		PkgBgInInputBuf = (PkgBgInInputBuf-ioDevice->PkgProcessor->PackageFormat->HeadLength)%ioDevice->InputBufSize;
	}
	/*-----------------------*/
	/* Sequence Matching End */
	/*-----------------------*/

                               /************************/
                               /*  FIND PACKAGE TAIL   */
                               /************************/
	/* TARGET:find out the byte after the last byte of raw data*/

	/* we can't locate the end of a package if both raw data size and tail are
	 * not specified. */
	if(ioDevice->PkgProcessor->RawDataSize==0 && ioDevice->PkgProcessor->PackageFormat->TailEnable==0)
	{
		COS_Logf(err,"find data package error:Both raw data size and package tail aren't specified.");
		return 0;
	}
	PkgEdInInputBuf = (PkgBgInInputBuf + ioDevice->PkgProcessor->PackageFormat->HeadLength +RawDataSize)%ioDevice->InputBufSize;
	ActualRawLen += RawDataSize;
	/*-------------------------*/
	/* Sequence Matching Start */
	/*-------------------------*/
	/* find package end byte with brute-force algorithm */
	uint8_t TempTailIndex = 0;
	/* find end byte regardless whether package tail is specified or not.If tail is not specified,
	 * what will be found is the byte after the last byte of raw data. */
	while(PkgEdInInputBuf != ioDevice->InputHeadIndex && TempTailIndex<ioDevice->PkgProcessor->PackageFormat->TailLength)
	{
		/* if data matches, continue finding */
		if(ioDevice->InputBuf[PkgEdInInputBuf]==ioDevice->PkgProcessor->PackageFormat->Tail[TempTailIndex])
		{
			/* The input buffer and tail sequence match the next byte in turn */
			PkgEdInInputBuf = (PkgEdInInputBuf+1)%ioDevice->InputBufSize;
			TempTailIndex++;
		}
		else // restart match
		{
			/* input buffer starts matching from next byte */
			PkgEdInInputBuf = (PkgEdInInputBuf-TempTailIndex+1)%ioDevice->InputBufSize;

			/* tail sequence starts matching from beginning */
			TempTailIndex=0;

			ActualRawLen++;
		}
	}
	/* if failed to find a tail sequence */
	if(TempTailIndex<ioDevice->PkgProcessor->PackageFormat->TailLength)
	{
		ioDevice->PkgProcessor->DataDst = NULL;
		/* remove all data */
		_io_InputBufRemove(ioDevice, _io_InputBufGetSize(ioDevice));
		return 0;
	}
	/* else head sequence found successful */
	else
	{
		PkgEdInInputBuf = (PkgEdInInputBuf-ioDevice->PkgProcessor->PackageFormat->TailLength)%ioDevice->InputBufSize;
	}/* the byte after last byte of raw data is already found when program runs here */
	/*-----------------------*/
	/* Sequence Matching End */
	/*-----------------------*/

	/* have both raw data size and package tail.
	 * --->see whether package tail sequence appears at correct location */
	if(ioDevice->PkgProcessor->RawDataSize!=0 && ioDevice->PkgProcessor->PackageFormat->TailEnable)
	{
		uint8_t i=0;
		for(i=0 ; i<ioDevice->PkgProcessor->PackageFormat->TailLength ; i++)
		{
			if(ioDevice->InputBuf[(PkgEdInInputBuf + i)%ioDevice->InputBufSize]!=ioDevice->PkgProcessor->PackageFormat->Tail[i])
			{/* if enters this program block, the package is almost damaged. */
				UselessDataLength += PkgSize;
				_io_InputBufRemove(ioDevice, UselessDataLength);
				return 0;
			}
		}
	}
	/*----so far we get the byte after last raw data byte */

                             /************************/
                             /*      CHECK DATA      */
                             /************************/
	_io_InputBufRemove(ioDevice, UselessDataLength);
	_io_InputBufRemove(ioDevice, ioDevice->PkgProcessor->PackageFormat->HeadLength);
	io_GetData(ioDevice, ioDevice->PkgProcessor->DataDst, ActualRawLen);
	_io_InputBufRemove(ioDevice, ChkValSize);
	_io_InputBufRemove(ioDevice, ioDevice->PkgProcessor->PackageFormat->TailLength);
	/* no check */
	if(!ioDevice->PkgProcessor->PackageFormat->CheckValEnable)
	{
		return 1;
	}

	/* do check */
	uint16_t icv = ioDevice->PkgProcessor->PackageFormat->CheckAlgorithm(ioDevice->PkgProcessor->DataDst,ioDevice->PkgProcessor->RawDataSize);
	if(!ioDevice->PkgProcessor->PackageFormat->TailEnable)
	{
		if(icv != ioDevice->InputBuf[PkgEdInInputBuf])
		{
			UselessDataLength += PkgSize;
			_io_InputBufRemove(ioDevice, UselessDataLength);
			return 0;
		}
	}
	/* tail enable */
	else
	{
		if(ioDevice->PkgProcessor->PackageFormat->TailBehindChk)
		{
			if(icv != ioDevice->InputBuf[PkgEdInInputBuf])
			{
				UselessDataLength += PkgSize;
				_io_InputBufRemove(ioDevice, UselessDataLength);
				return 0;
			}
		}
		else
		{
			if(icv != ioDevice->InputBuf[(PkgEdInInputBuf+ioDevice->PkgProcessor->PackageFormat->TailLength)%ioDevice->InputBufSize])
			{
				UselessDataLength += PkgSize;
				_io_InputBufRemove(ioDevice, UselessDataLength);
				return 0;
			}
		}
	}

	/* check pass or no check */
	return 1;
}


/*****************************************************************************************************
 * @name:ExamPkgFmt
 * @brief:examine whether the package format is legal.
 * @params:
 *     1.PkgFmt:pointer of package format definition struct.
 * @retval: 1 if package format is correct ; 0 if package format is illegal.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t ExamPkgFmt(COS_PkgFmt *PkgFmt)
{
	if(PkgFmt==NULL)
		return 0;

	/* enable package head but head information is not correct */
	else if(PkgFmt->HeadEnable&&(PkgFmt->Head==NULL||PkgFmt->HeadLength==0))
		return 0;

	/* enable package tail but tail information is not correct */
	else if(PkgFmt->TailEnable&&(PkgFmt->Tail==NULL||PkgFmt->TailLength==0))
		return 0;

	else if(PkgFmt->CheckValEnable&&PkgFmt->CheckAlgorithm==NULL)
		return 0;

	return 1;
}

uint16_t Chk_XOR(uint8_t *pData, uint32_t DataLen)
{
	uint8_t icv=0;
	uint32_t i=0;
	for(i=0 ; i<DataLen ; i++)
	{
		icv^=pData[i];
	}
	return (uint16_t)icv;
}

/****************************Info**********************************************
 * Name:    CRC-16/CCITT        x16+x12+x5+1
 * Width:	16
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-CCITT,CRC-16/CCITT-TRUE,CRC-16/KERMIT
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_CCITT(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x1021;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
//这里为了效率，我们不需要将所有Refin和refout为true的输入输出数据移位转换
//只需要将poly二项式转换后，运算时将左移变为右移
uint16_t Chk_CRC16_CCITT(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x1021;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin);
}
#endif
/****************************Info**********************************************
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1
 * Width:	16
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
uint16_t Chk_CRC16_CCITT_FALSE(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x1021;

	while (DataLen--)
	{
		wCRCin ^= *(pData++) << 8;
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}
/****************************Info**********************************************
 * Name:    CRC-16/XMODEM       x16+x12+x5+1
 * Width:	16
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN
 *****************************************************************************/
uint16_t Chk_CRC16_XMODEM(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x1021;

	while (DataLen--)
	{
		wCRCin ^= (*(pData++) << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}
/****************************Info**********************************************
 * Name:    CRC-16/X25          x16+x12+x5+1
 * Width:	16
 * Poly:    0x1021
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0XFFFF
 * Note:
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_X25(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x1021;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF);
}
#else
uint16_t Chk_CRC16_X25(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x1021;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin^0xFFFF);
}
#endif
/****************************Info**********************************************
 * Name:    CRC-16/MODBUS       x16+x15+x2+1
 * Width:	16
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Note:
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_MODBUS(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x8005;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
uint16_t Chk_CRC16_MODBUS(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x8005;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin);
}
#endif
/****************************Info**********************************************
 * Name:    CRC-16/IBM          x16+x15+x2+1
 * Width:	16
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0x0000
 * Alias:   CRC-16,CRC-16/ARC,CRC-16/LHA
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_IBM(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x8005;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin);
}
#else
uint16_t Chk_CRC16_IBM(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x8005;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin);
}
#endif
/****************************Info**********************************************
 * Name:    CRC-16/MAXIM        x16+x15+x2+1
 * Width:	16
 * Poly:    0x8005
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_MAXIM(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x8005;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF);
}
#else
uint16_t Chk_CRC16_MAXIM(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x8005;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin^0xFFFF);
}
#endif
/****************************Info**********************************************
 * Name:    CRC-16/USB          x16+x15+x2+1
 * Width:	16
 * Poly:    0x8005
 * Init:    0xFFFF
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Note:
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_USB(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x8005;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF);
}
#else
uint16_t Chk_CRC16_USB(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0xFFFF;
	uint16_t wCPoly = 0x8005;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin^0xFFFF);
}
#endif
/****************************Info**********************************************
 * Name:    CRC-16/DNP          x16+x13+x12+x11+x10+x8+x6+x5+x2+1
 * Width:	16
 * Poly:    0x3D65
 * Init:    0x0000
 * Refin:   True
 * Refout:  True
 * Xorout:  0xFFFF
 * Use:     M-Bus,ect.
 *****************************************************************************/
#if 0
uint16_t Chk_CRC16_DNP(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x3D65;
	uint8_t wChar = 0;

	while (DataLen--)
	{
		wChar = *(pData++);
		InvertUint8(&wChar,&wChar);
		wCRCin ^= (wChar << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin,&wCRCin);
	return (wCRCin^0xFFFF) ;
}
#else
uint16_t Chk_CRC16_DNP(uint8_t *pData, uint32_t DataLen)
{
	uint16_t wCRCin = 0x0000;
	uint16_t wCPoly = 0x3D65;

	InvertUint16(&wCPoly,&wCPoly);
	while (DataLen--)
	{
		wCRCin ^= *(pData++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = (wCRCin >> 1);
		}
	}
	return (wCRCin^0xFFFF);
}
#endif
/****************************Info**********************************************
 * Name:    InvertUint8
 * Note: 	把字节颠倒过来，如0x12变成0x48
			0x12: 0001 0010
			0x48: 0100 1000
 *****************************************************************************/
void InvertUint8(uint8_t *dBuf,uint8_t *srcBuf)
{
	int i;
	uint8_t tmp[4]={0};

	for(i=0;i< 8;i++)
	{
		if(srcBuf[0]& (1 << i))
		tmp[0]|=1<<(7-i);
	}
	dBuf[0] = tmp[0];

}
void InvertUint16(uint16_t *dBuf,uint16_t *srcBuf)
{
	int i;
	uint16_t tmp[4]={0};

	for(i=0;i< 16;i++)
	{
		if(srcBuf[0]& (1 << i))
		tmp[0]|=1<<(15 - i);
	}
	dBuf[0] = tmp[0];
}
void InvertUint32(uint32_t *dBuf,uint32_t *srcBuf)
{
	int i;
	uint32_t tmp[4]={0};

	for(i=0;i< 32;i++)
	{
		if(srcBuf[0]& (1 << i))
		tmp[0]|=1<<(31 - i);
	}
	dBuf[0] = tmp[0];
}


#ifdef __cplusplus
}
#endif

