/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_io.c
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
#include "./src/c_includes.h"
#include "./src/debug/c_debug.h"
#include "./src/communication/c_io.h"
#include "./src/app/c_app.h"
#include "./src/app/c_builtinapp.h"
#include "./src/car/c_car.h"

/* CarOS built-in io device usart handler */
#if USE_HAL_LIB
#define cosiohuart &huart1
extern UART_HandleTypeDef huart1;
#elif USE_FW_LIB
/* notice that USARTx is pointer rather than  */
#define cosiohuart USART1 // only need to modify here.
extern USART_TypeDef* USART1;
#elif USE_MSP432
#define cosiohuart EUSCI_A0_BASE
#endif

/* CarOS built-in io device buffer  */
uint8_t COS_InputBuffer[1024*INPUT_BUFFER_SIZE];
uint8_t COS_OutputBuffer1[1024*OUTPUT_BUFFER1_SIZE];
uint8_t COS_OutputBuffer2[1024*OUTPUT_BUFFER2_SIZE];

/* system built-in io device */
COS_io cosio;

/* CarOS io device mount point. maximum of 6 io device can be mounted */
COS_io *_iob[MAXIODEVICEAMOUNT]={&cosio,0};





static void _io_InputBufRetreatPointer(COS_io *ioDevice);
static void _io_InputBufAdvancePointer(COS_io *ioDevice);

/*****************************************************************************************************
 * @name:cosioInit
 * @brief:Initialize system built-in io device and mount points.
 * @params:none
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void cosioInit()
{
	/* set all mount point as NULL */
	for(uint8_t i=0 ; i<6 ; i++)
	{
		_iob[i] = NULL;
	}
	/* mount cosio to system io mount point and initialize io device */
	io_Init(&cosio, "cosio", cosiohuart, COS_InputBuffer, INPUT_BUFFER_SIZE*1024, COS_OutputBuffer1, OUTPUT_BUFFER1_SIZE*1024, COS_OutputBuffer2, OUTPUT_BUFFER2_SIZE*1024);

	/* load system built-in applications */
	LoadApp(&cosio, BuiltinAppList, BUILT_IN_APP_AMOUNT);

	/* load car applications */
	LoadApp(&cosio, CarAppList,CARAPPAMOUNT);
	_io_Transmit(&cosio, (uint8_t*)"console uart test\r\n", 19);
	COS_printf("Console ready.\r\n");

}



/*****************************************************************************************************
 * @name:COS_printf
 * @brief:an alternative to printf()
 * @params:
 *     1.fmt:
 *     2.(optional parameters):the value of format params in string 'fmt'.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_printf(const char *fmt,...)
{
	va_list ap;
	va_start(ap,fmt);
	io_vprintf(&cosio,fmt,ap);
	va_end(ap);
}


/*****************************************************************************************************
 * @name:io_Init
 * @brief:Mount io device to system mount point and initialize the io device.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.Name:string of io device name.
 *     3.huart:pointer of usart.
 *     4.InputBuf:pointer of input buffer.
 *     5.InputBufLen:length of input buffer.
 *     6.OutputBuf1:pointer of output buffer1.
 *     7.OutputBuf1Len:length of output buffer1.
 *     8.OutputBuf2:pointer of output buffer2.
 *     9.OutputBuf2Len:length of output buffer2.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/

void io_Init(COS_io *ioDevice, const char* Name, COS_uart huart, uint8_t *InputBuf, uint32_t InputBufLen, uint8_t *OutputBuf1, uint32_t OutputBuf1Len, uint8_t *OutputBuf2, uint32_t OutputBuf2Len)
{
	COS_assert((InputBuf && InputBufLen),warning,"%s's input buffer is empty.",Name);
	COS_assert((OutputBuf1 && OutputBuf1Len),warning,"%s's output buffer1 is empty.",Name);
	COS_assert((OutputBuf2 && OutputBuf2Len),warning,"%s's output buffer2 is empty.",Name);

	if(ioDevice==NULL)
		return ;

	/* mount io device on system */
	for(uint8_t i=0 ; i<6 ; i++)
	{
		if(_iob[i]==NULL)
		{
			_iob[i] = ioDevice;
			break;
		}
	}

	if(Name == NULL)
	{
		COS_Logf(info,"Init io device without name, now rename it 'nameless io device'");
		ioDevice->Name = "nameless io device";
	}
	else
	{
		ioDevice->Name = Name;
	}
	ioDevice->huart = huart;

	/* input buffer init */
	if((InputBuf!=NULL) && (InputBufLen!=0))
	{
		ioDevice->InputBuf = InputBuf;
		ioDevice->InputBufSize = InputBufLen;
		memset(InputBuf,0,InputBufLen);

#if USE_HAL_LIB
		/* it's necessary to start receive interrupt when
		 * using HAL lib */
//		HAL_UART_Receive_IT(ioDevice->huart,&(ioDevice->RxTemp),1);
		__HAL_UART_ENABLE_IT(ioDevice->huart,UART_IT_RXNE);
#elif USE_FW_LIB
#elif USE_MSP432
#endif


	}
	else
	{
		ioDevice->InputBuf = NULL;
		ioDevice->InputBufSize = 0;
	}
	ioDevice->InputHeadIndex = 0;
	ioDevice->InputTailIndex = 0;
	ioDevice->InputBufFull = 0;

	/* output buffer1 init */
	if((OutputBuf1!=NULL) && (OutputBuf1Len!=0))
	{
		ioDevice->OutputBuf[0] = OutputBuf1;
		ioDevice->OutputBufSize[0] = OutputBuf1Len;
		memset(OutputBuf1,0,OutputBuf1Len);
	}
	else
	{
		ioDevice->OutputBuf[0] = NULL;
		ioDevice->OutputBufSize[0] = 0;
	}
	ioDevice->OutputBufPosition[0] = 0;


	/* output buffer2 init */
	if((OutputBuf2!=NULL) && (OutputBuf2Len!=0))
	{
		ioDevice->OutputBuf[1] = OutputBuf2;
		ioDevice->OutputBufSize[1] = OutputBuf2Len;
		memset(OutputBuf2,0,OutputBuf2Len);
	}
	else
	{
		ioDevice->OutputBuf[1] = NULL;
		ioDevice->OutputBufSize[1] = 0;
	}
	ioDevice->OutputBufPosition[1] = 0;

	/* set current activated buffer as buffer 0 which is
	 * main output buffer */
	ioDevice->ActivatedOutBufIndex = 0;

	/* io device doesn't transmit data at first */
	ioDevice->Outputing = 0;

	/* clear command list */
	ioDevice->CmdList = NULL;

	/* clear io stream */
	ioDevice->PkgProcessor = NULL;

	/* set io device to command mode as default */
	ioDevice->ReceiveMode = 0;


}


/*****************************************************************************************************
 * @name:io_printf
 * @brief:print format string through io device.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.fmt:formatted string
 *     3.(optional parameter)
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_printf(COS_io *ioDevice,const char *fmt,...)
{
	/* create a container to contain parameters */
	va_list ap;
	va_start(ap,fmt);
	io_vprintf(ioDevice,fmt,ap);
	va_end(ap);
}



/*****************************************************************************************************
 * @name:io_Process
 * @brief:process all io device input and output data.
 * @params:none
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_Process()
{
	for(uint8_t i=0 ; i<MAXIODEVICEAMOUNT; i++)
	{
		if(_iob[i]!=NULL)
		{
			io_OutputProcess(_iob[i]);
			io_InputProcess(_iob[i]);
		}
	}
}



/*****************************************************************************************************
 * @name:io_CommandMode
 * @brief:start io device receive command mode, which enables io device to receive command
 * @params:
 *     1.ioDevice:pointer of io device
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_CommandMode(COS_io *ioDevice)
{
	ioDevice->ReceiveMode = COMMAND_MODE;
	io_PkgProcessorDeinit(ioDevice);
}
/*****************************************************************************************************
 * @name:cosioTransmitOver
 * @brief:set member of 'Outputing' of cosio to 0, which indicates io device transmit over.
 * @params:none
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void cosioTransmitOver()
{
	cosio.Outputing = 0;
}


/*****************************************************************************************************
 * @name:io_DMAHandler
 * @brief:set member of 'Outputing' of an ioDevice to 0, which indicates io device transmit over.
 * @params:
 *     1.huart:pointer of uart instance.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_DMAHandler(COS_uart huart)
{
	uint8_t i=0;
	for(i=0 ; i<MAXIODEVICEAMOUNT ; i++)
	{
		if(_iob[i]==NULL)
			continue;
		else if(huart==_iob[i]->huart)
		{
			_iob[i]->Outputing=0;
		}
	}
}


/*****************************************************************************************************
 * @name:io_OutputProcess
 * @brief:check whether there is data in output buffers waiting for transmitting and adjust main
 *     output buffer dynamically.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_OutputProcess(COS_io *ioDevice)
{
	/* if io device is transmitting data, it won't transmit data again. */
	if(ioDevice->Outputing)
	{
		return ;
	}

	/* if current writable buffer has some data to transmit,
	 * the io device start transmission.
	 * if current writable buffer is indeed empty, then check whether current
	 * buffer is secondary output buffer. if it's secondary buffer and main
	 * buffer is empty, current waritable buffer will be set to main buffer.
	 * That is dynamic main buffer. */
	if(!_io_ActivatedBufEmpty(ioDevice))
	{
		/* set transmission flag as 1, which means the io device is transmitting */
		ioDevice->Outputing = 1;

		/* start transmission */
		_io_Transmit(ioDevice, ioDevice->OutputBuf[ioDevice->ActivatedOutBufIndex], ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]);

		/* clear output buffer */
		ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex] = 0;

		/* set current writable buffer as another one */
		ioDevice->ActivatedOutBufIndex = 1 - ioDevice->ActivatedOutBufIndex;
	}
	else
	{
		/* Current Activated buffer is secondary buffer and primary buffer is empty */
		if((ioDevice->ActivatedOutBufIndex == 1)&&(ioDevice->OutputBufPosition[0]==0))
		{
			/* set current writable buffer as main buffer */
			ioDevice->ActivatedOutBufIndex = 0;
		}
	}
}

/*****************************************************************************************************
 * @name:io_InputProcess
 * @brief:process input data, mostly process the commands that are input into input buffer of io device.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_InputProcess(COS_io *ioDevice)
{
	if(io_InputBufEmpty(ioDevice))
		return ;

	if(ioDevice->ReceiveMode==PACKAGE_MODE)
	{
		while(!io_InputBufEmpty(ioDevice))
		{
			io_PackageProcess(ioDevice);
		}
	}
	else if(ioDevice->ReceiveMode==COMMAND_MODE)
	{
		char Cmd[256] = {0};
		uint32_t CmdLineLength = 0;

		/* io_GetLine() returns 0 when no command is got */
		CmdLineLength = io_GetLine(ioDevice, (uint8_t*)Cmd, 256);
		if(CmdLineLength!=0)
		{
			io_CmdProcess(ioDevice->CmdList, Cmd);
		}
	}
	else
	{
		return;
	}

}


/*****************************************************************************************************
 * @name:io_vprintf
 * @brief:generate formatted data and write data to current activated buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.fmt:formatted string.
 *     3.ap:parameter list of formatted string.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_vprintf(COS_io *ioDevice,const char *fmt, va_list ap)
{
	uint8_t *DataDst = NULL;
	uint32_t OutBufSpareSpace = 0;

	if(ioDevice->OutputBuf[0]== NULL || ioDevice->OutputBuf[1]==NULL)
	{
		if(ioDevice != &cosio)
		{
			COS_Logf(err,"write output buffer failed:io device '%s' has no output buffer, please check its output buffer.");
		}
		else
		{
			_io_Transmit(ioDevice, (uint8_t*)"cosio print failed:has no output buffer.\r\n", 36);
		}
		return ;
	}

	/* get valid idle output buffer pointer */
	OutBufSpareSpace = _io_GetSpareOutBuf(ioDevice, &DataDst);

	/* if there is no more space to write in */
	if(OutBufSpareSpace == 0)
	{
		/* exit function and write nothing into output buffer */
		COS_Logf(err,"CarOS print failed:output buffer is full.");
		return ;
	}

	/* write formatted string to destination buffer */
	vsnprintf((char*)DataDst,OutBufSpareSpace,fmt,ap);
	ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex] +=
			strlen((char*)(ioDevice->OutputBuf[ioDevice->ActivatedOutBufIndex] + ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]));

	/* if activated buffer is full, the data in current
	 * activated buffer must be sent right now */
	if(_io_GetSpareOutBuf(ioDevice, &DataDst)==1)
	{
		io_OutputProcess(ioDevice);
	}
}
/*****************************************************************************************************
 * @name:io_UartIntCallback
 * @brief:Process all uart receive interrupts. Put this function at every uart receive interrupt
 *     function.
 * @params:
 *     1.huart:pointer of uart instance, varing from platform to platform.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_UartRxIntHandler(COS_uart huart)
{
#if USE_HAL_LIB
	uint8_t i=0;
	for(i=0 ; i<MAXIODEVICEAMOUNT ; i++)
	{
		if( _iob[i]!=NULL && huart == _iob[i]->huart )
		{
			if(huart->Instance->SR & 0x00000020)
			{
				io_InputBufWrite(_iob[i],(uint8_t*)&huart->Instance->DR,1);
			}
		}
	}
//	HAL_UART_Receive_IT(_iob[i]->huart,&(_iob[i]->RxTemp),1);
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);
#elif USE_FW_LIB

#elif USE_MSP432

#endif
}
/*****************************************************************************************************
 * @name:io_SendData
 * @brief:write binary data to output buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pData:pointer of source data.
 *     3.Length:length of source data.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_SendData(COS_io *ioDevice, void *pData, uint32_t Length)
{
	uint8_t *DataDst = NULL;
	uint32_t OutBufSpareSpace = 0;
	uint32_t DataSentLen = 0;       // length of data was sent already.

	if(ioDevice->OutputBuf[0]== NULL || ioDevice->OutputBuf[1]==NULL)
	{
		if(ioDevice != &cosio)
		{
			COS_Logf(err,"write output buffer failed:io device '%s' has no output buffer, please check its output buffer.");
		}
		else
		{
			_io_Transmit(ioDevice, (uint8_t*)"print failed:has no output buffer.\r\n", 36);
		}
		return ;
	}
	/* get valid idle output buffer pointer */
	OutBufSpareSpace = _io_GetSpareOutBuf(ioDevice, &DataDst);

	/* if there is no more space to write in */
	if(OutBufSpareSpace == 0)
	{
		/* exit function and write nothing into output buffer */
		COS_Logf(err,"CarOS print failed:output buffer is full.");
		return ;
	}

	if(Length>OutBufSpareSpace)
	{
		memcpy(DataDst,pData,OutBufSpareSpace);
		io_OutputProcess(ioDevice);
		DataSentLen = OutBufSpareSpace;
		OutBufSpareSpace = _io_GetSpareOutBuf(ioDevice, &DataDst);
		memcpy(DataDst,pData + DataSentLen,OutBufSpareSpace);
		ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]+=OutBufSpareSpace;
	}
	else
	{
		memcpy(DataDst,pData,Length);
		ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]+=Length;
	}

}


/*****************************************************************************************************
 * @name:_io_GetSpareOutBuf
 * @brief:get spare space of current output buffer and the pointer of spare space.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pDataDst:the address of pointer of spare buffer.
 * @retval: the spare space size of current activated output buffer. unit:byte
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint32_t _io_GetSpareOutBuf(COS_io *ioDevice, uint8_t **pDataDst)
{
	if(ioDevice->OutputBuf[0]==NULL || ioDevice->OutputBuf[1]==NULL)
	{
		return 0;
	}

	/* since 'ActivatedOutBufIndex' can only be 0 or 1, the check step
	 * guarantees io device runs correctly. */
	if(ioDevice->ActivatedOutBufIndex <2)
	{
		/* the address of first byte of spare space is buffer start address plus  */
		*pDataDst = (ioDevice->OutputBuf[ioDevice->ActivatedOutBufIndex] + ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]);
		return (ioDevice->OutputBufSize[ioDevice->ActivatedOutBufIndex] - ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]);
	}
	else
	{
		COS_Logf(info,"can't get spare output buffer, io device '%s' has wrong \"ActivatedOutBufIndex\".",ioDevice->Name);
		return 0;
	}
}



/*****************************************************************************************************
 * @name:_io_ActivatedBufEmpty
 * @brief:if current activated buffer is empty return 1, otherwise return 0
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: 1 if empty, 0 if not empty.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_ActivatedBufEmpty(COS_io *ioDevice)
{
	return (ioDevice->OutputBufPosition[ioDevice->ActivatedOutBufIndex]==0);
}




/*****************************************************************************************************
 * @name:_io_Transmit
 * @brief:Start transmission through usart.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pData:the data start address.
 *     3.Length:length of data to transmit.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void _io_Transmit(COS_io *ioDevice, uint8_t *pData, uint32_t Length)
{
#if	USE_HAL_LIB
#if USE_USART_DMA
	HAL_UART_Transmit_DMA(ioDevice->huart, pData, Length);
#else
	HAL_UART_Transmit(ioDevice->huart, pData, Length, 0xffff);
	ioDevice->Outputing=0;
#endif
#elif USE_FW_LIB

#elif USE_MSP432

#endif
}

/*****************************************************************************************************
 * @name:io_InputBufWrite
 * @brief:write data of a specified length.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pData:data start address.
 *     3.Length:data length.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_InputBufWrite(COS_io *ioDevice, uint8_t *pData, uint32_t Length)
{
	uint32_t RestDataLen = Length;
	uint32_t DataIndex = 0;
	/* if circular buffer is full, it won't be written any data */
	if(ioDevice->InputBufFull)
		return ;
	else if(ioDevice->InputBuf == NULL)
	{
		COS_Logf(err,"failed to write io device '%s' :No input buffer.",ioDevice->Name);
		return;
	}


	/* Write data byte by byte */
	while(RestDataLen)
	{
		_io_InputBufWriteByte(ioDevice, *(pData + DataIndex));
		RestDataLen--;
		DataIndex++;
	}
}



/*****************************************************************************************************
 * @name:_io_InputBufWriteByte
 * @brief:write a byte into input buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.Data:raw data.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void _io_InputBufWriteByte(COS_io *ioDevice, uint8_t Data)
{
	if(ioDevice->ReceiveMode==COMMAND_MODE && Data==0)
		return ;

	/* load one byte data and move the pointer of circular buffer */
	*(ioDevice->InputBuf + ioDevice->InputHeadIndex) = Data;
	_io_InputBufAdvancePointer(ioDevice);
}




/*****************************************************************************************************
 * @name:io_GetLine
 * @brief:read a string end of '\n' and write the string into 'pDataDst'.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pDataDst:start address of data destination.
 *     3.DstLength:the capacity of destination.
 * @retval: actual length of string that be written to destination.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint32_t io_GetLine(COS_io *ioDevice, uint8_t *pDataDst, uint32_t DstLength)
{
	uint32_t DataLength = 0;
	uint32_t InputDataIndex = ioDevice->InputTailIndex;

	if(io_InputBufEmpty(ioDevice))
	{
		return 0;
	}
	/* line end signal is found or buffer end */
	while(ioDevice->InputBuf[InputDataIndex]!='\n' && InputDataIndex != ioDevice->InputHeadIndex)
	{
		DataLength++;
		InputDataIndex = (InputDataIndex + 1) % (ioDevice->InputBufSize);
	}

	/* if read the end of buffer */
	if(InputDataIndex == ioDevice->InputHeadIndex)
	{
		*pDataDst = 0;
		return 0;
	}
	else // '\n' is found
	{
		uint32_t RealDataLen;
		if(DataLength>DstLength)
		{
			/* write part of original string to destination */
			RealDataLen = io_GetData(ioDevice,pDataDst,DstLength);

			/* remove rest of original data and '\n' */
			io_InputBufRemove(ioDevice,DataLength-DstLength+1);
		}
		else
		{
			/* write a line to destination */
			RealDataLen = io_GetData(ioDevice,pDataDst,DataLength);

			/* remove last '\n' */
			io_InputBufRemoveByte(ioDevice);
		}

		return RealDataLen;
	}
}





/*****************************************************************************************************
 * @name:io_GetData
 * @brief:write data of specified length to destination and remove the data from input buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pDataDst:pointer of data destination.
 *     3.Length:The length of the data to be got.
 * @retval: actual obtained data length.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint32_t io_GetData(COS_io *ioDevice, uint8_t *pDataDst, uint32_t Length)
{
	uint32_t RestDataLen = Length;
	uint32_t DataIndex = 0;

	/* if this io device doesn't have input device, operation of getting data
	 * is not allowed. */
	if(ioDevice->InputBuf == NULL)
	{
		COS_Logf(err,"can't get data from io device, '%s' has no input buffer.",ioDevice->Name);
		return 0;
	}

	/* read data byte by byte */
	while(RestDataLen)
	{
		if(!_io_GetByte(ioDevice,pDataDst+DataIndex))
		{
			break;
		}
		RestDataLen--;
		DataIndex++;
	}
	return DataIndex;
}





/*****************************************************************************************************
 * @name:_io_GetByte
 * @brief:get one byte from input buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.pDataDst:pointer of data destination.
 * @retval: 1 if read successfully, 0 if read error.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_GetByte(COS_io *ioDevice, uint8_t *pDataDst)
{
	uint8_t Error = 0;
	if(!io_InputBufEmpty(ioDevice))
	{
		*pDataDst = ioDevice->InputBuf[ioDevice->InputTailIndex];

		/* no need to remove the data, just retreat pointer is OK */
		_io_InputBufRetreatPointer(ioDevice);

		Error = 1;
	}
	return Error;
}





/*****************************************************************************************************
 * @name:io_InputBufRemove
 * @brief:remove data of specified length.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.Length:length of data to be removed.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_InputBufRemove(COS_io *ioDevice, uint32_t Length)
{
	for(uint8_t i=0 ; i<Length ; i++)
	{
		if(io_InputBufRemoveByte(ioDevice)==0)
		{
			break;
		}
	}
}




/*****************************************************************************************************
 * @name:io_InputBufRemoveByte
 * @brief:remove one byte from input buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: 1 if remove successfully, 0 if remove fail.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t io_InputBufRemoveByte(COS_io *ioDevice)
{
	uint8_t Error = 0;
	if(!io_InputBufEmpty(ioDevice))
	{
		_io_InputBufRetreatPointer(ioDevice);
		Error = 1;
	}
	return Error;
}




/*****************************************************************************************************
 * @name:io_InputBufReset
 * @brief:Reset and clear input buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_InputBufReset(COS_io *ioDevice)
{
	ioDevice->InputHeadIndex = 0;
	ioDevice->InputTailIndex = 0;
	ioDevice->InputBufFull = 0;
}



/*****************************************************************************************************
 * @name:io_InputBufFull
 * @brief:check whether input buffer is full.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: 1 if full, 0 if not full.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t io_InputBufFull(COS_io *ioDevice)
{
	return ioDevice->InputBufFull;
}



/*****************************************************************************************************
 * @name:io_InputBufEmpty
 * @brief:check whether input buffer is empty.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: 1 if empty, 0 if not empty.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t io_InputBufEmpty(COS_io *ioDevice)
{
	return (!ioDevice->InputBufFull && (ioDevice->InputHeadIndex == ioDevice->InputTailIndex));
}



/*****************************************************************************************************
 * @name:io_InputBufCapacity
 * @brief:get input buffer capacity.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: the capacity of input buffer.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint32_t io_InputBufCapacity(COS_io *ioDevice)
{
	return ioDevice->InputBufSize;
}



/*****************************************************************************************************
 * @name:io_InputBufGetSize
 * @brief:get current occupied space size.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: the current occupied space size.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint32_t io_InputBufGetSize(COS_io *ioDevice)
{
	int32_t Size = ioDevice->InputBufSize;
	if(!(ioDevice->InputBufFull))
	{
		if(ioDevice->InputHeadIndex >= ioDevice->InputTailIndex)
		{
			Size = ioDevice->InputHeadIndex - ioDevice->InputTailIndex;
		}
		else
		{
			Size = ioDevice->InputBufSize - ioDevice->InputTailIndex + ioDevice->InputHeadIndex;
		}
	}
	return Size;
}



/*****************************************************************************************************
 * @name:io_InputBufGetSpare
 * @brief:get size of spare space of input buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: spare space size.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint32_t io_InputBufGetSpare(COS_io *ioDevice)
{
	int32_t SpareSize = ioDevice->InputBufSize;
	if(!(ioDevice->InputBufFull))
	{
		if(ioDevice->InputHeadIndex >= ioDevice->InputTailIndex)
		{
			SpareSize = ioDevice->InputBufSize - ioDevice->InputHeadIndex + ioDevice->InputTailIndex;
		}
		else
		{
			SpareSize = ioDevice->InputTailIndex - ioDevice->InputHeadIndex;
		}
	}
	return SpareSize;
}



/*****************************************************************************************************
 * @name:_io_InputBufRetreatPointer
 * @brief:retreat pointer of ring buffer, which means remove one byte of ring buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void _io_InputBufRetreatPointer(COS_io *ioDevice)
{
	ioDevice->InputBufFull = 0;
	ioDevice->InputTailIndex = (ioDevice->InputTailIndex + 1) % ioDevice->InputBufSize;
}



/*****************************************************************************************************
 * @name:_io_InputBufAdvancePointer
 * @brief:advance pointer of ring buffer, which means add one byte of ring buffer.
 * @params:
 *     1.ioDevice:pointer of io device.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void _io_InputBufAdvancePointer(COS_io *ioDevice)
{
	if(!ioDevice->InputBufFull)
	{
		ioDevice->InputHeadIndex = (ioDevice->InputHeadIndex + 1) % ioDevice->InputBufSize;
		ioDevice->InputBufFull = (ioDevice->InputHeadIndex == ioDevice->InputTailIndex);
	}
}

#ifdef __cplusplus
}
#endif

