/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_command.c
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


/*****************************************************************************************************
 * @name:io_AddCmd
 * @brief:allocate a block of memory for 'COS_Cmd' struct and load parameters to it. The
 *     'ParamsContainer' pointer of 'COS_Cmd' struct will point to a piece of memory that is allocated
 *     dynamically according to the string 'ParamsDescription'.
 * @params:
 *     1.ioDevice:pointer of io device.
 *     2.CmdName:name of command
 *     3.ParamsDescription:a string that describes parameters type, which are divided by ','.
 *     4.CmdCallbackFunc:command executive function.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_AddCmd(COS_io *ioDevice, const char *CmdName, const char *ParamsDescription, void* CmdCallbackFunc)
{
	COS_Cmd *NewCmd;
	char *pToken = NULL;                 // divided string of param type
	char *pDivideTemp = NULL;            // for the third param of strtok_r()
	static const char DivideSep[]=" ,";  // a string contains separator character.
	uint16_t ParamSize = 0;              // total size of all parameters.
	uint8_t *ParamContainer = NULL;      // dynamically allocated memory pointer.

	char ParamsDscpCopy[256] = {0};      /* since strtok_r() doesn't accept const char* type, it is used to
	                                      * carry original data of const char* type 'ParamsDescription' */

	/* allocate a piece of memory */
	NewCmd = (COS_Cmd*)cos_malloc(sizeof(COS_Cmd));

	if(NewCmd==NULL) // memory allocation failed
	{
		COS_Logf(err,"create command '%s' fail:malloc fail.",CmdName);
		return ;
	}
	else  // memory allocation succeed
	{
		NewCmd->CmdName = CmdName;
		NewCmd->ParamsDescription = ParamsDescription;
		NewCmd->CmdCallbackFunc = CmdCallbackFunc;
		NewCmd->pNext = NULL;

		/* mount new command object to ioDevice */
		if(ioDevice->CmdList==NULL)
		{
			ioDevice->CmdList = NewCmd;
		}
		else
		{
			NewCmd->pNext = ioDevice->CmdList;
			ioDevice->CmdList = NewCmd;
		}

		/* if this is a command without parameter */
		if(ParamsDescription == NULL || *ParamsDescription == '\0')
		{
			NewCmd->ParamsContainer = NULL;
			return ;
		}

		/* since strtok_r() doesn't accept constant string, the string variable
		 * which is a copy of original string is a must. */
		strncpy(ParamsDscpCopy,ParamsDescription, 256);

		/* get first parameter type */
		pToken = strtok_r(ParamsDscpCopy,DivideSep,&pDivideTemp);

		while(pToken!=NULL)
		{
			ParamSize += _io_GetParamSizeByStr(pToken);
			pToken = strtok_r(NULL,DivideSep,&pDivideTemp);
		}
		ParamContainer = (uint8_t*)cos_malloc(ParamSize);
		if(ParamContainer==NULL)
		{
			COS_Logf(warning,"command '%s' allocate parameter container failed:memory not enough.",CmdName);
		}
		NewCmd->ParamsContainer = ParamContainer;
	}
}



/*****************************************************************************************************
 * @name:io_CmdProcess
 * @brief:seek for command and read value of parameters to parameter container and call callback func.
 * @params:
 *     1.CmdList:a chain of struct 'COS_Cmd'.
 *     2.CmdStr:original command string.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_CmdProcess(COS_Cmd *CmdList, char *CmdStr)
{
	COS_Cmd *pCmdTemp = CmdList;        // command list operating pointer.
	char *pCmdStrToken = NULL;          // get sub string of 'CmdStr'.
	char *pDivideTemp1 = NULL;          // for the third param of strtok_r().
	char *pParamTypeToken = NULL;       // get param type from param description string.
	char *pDivideTemp2 = NULL;          // for the third param of strtok_r().
	const char *DivideSep = " ,\r";
	uint8_t ParamType = 0;              // current parameter type when traversing.
	uint16_t CurrentParamsSizeSum = 0;  // current total size of parameters.
	int ParamOperationErrorCode = 0;    // the error code from scanwrite function.

	/* check whether any of params is illegal */
	if((CmdList==NULL) || (CmdStr==NULL))
	{
		return ;
	}
	pCmdStrToken = CmdStr;

	/* get command name */
	pCmdStrToken = strtok_r(CmdStr,DivideSep,&pDivideTemp1);

	/* find command in the command list*/
	while(pCmdTemp!=NULL)
	{
		if(strstr(pCmdStrToken, pCmdTemp->CmdName))
		{
			break;
		}
		else
		{
			pCmdTemp = pCmdTemp->pNext;
		}
	}
	if(pCmdTemp==NULL)
	{
		COS_Logf(err,"%s : No such command.",pCmdStrToken);
		return ;
	}

	/* if the target command doesn't need parameter, directly
	 * call callback function */
	if(pCmdTemp->ParamsDescription==NULL)
	{
		io_CmdExecute(pCmdTemp);
		return ;
	}
	/* get first param value  */
	pCmdStrToken = strtok_r(NULL,DivideSep,&pDivideTemp1);

	/* since string to be split for strtok_r() must be char* type rather
	 * than const char* type, the string must be copied to char* type variable. */
	char ParamsDscpCopy[256] = {0};
	strncpy(ParamsDscpCopy, pCmdTemp->ParamsDescription, 256);

	/* get first param type string */
	pParamTypeToken = strtok_r(ParamsDscpCopy,DivideSep,&pDivideTemp2);

	/* traverse all params */
	while(pCmdStrToken!=NULL)
	{
		/* get to know what type it is */
		ParamType = _io_GetParamType(pParamTypeToken);

		/* put data into paramlist in order */
		ParamOperationErrorCode = _io_ScanWriteParamData(pCmdStrToken, ParamType, (pCmdTemp->ParamsContainer+CurrentParamsSizeSum));

		if(ParamOperationErrorCode<=0) // scan
		{
			COS_Logf(err,"can't load parameters correctly, check your command parameters.");
			return ;
		}
		CurrentParamsSizeSum += _io_GetParamSizeByTypeCode(ParamType);

		/* get next param value */
		pCmdStrToken = strtok_r(NULL,DivideSep,&pDivideTemp1);

		/* get next param type */
		pParamTypeToken = strtok_r(NULL,DivideSep,&pDivideTemp2);
	}
	io_CmdExecute(pCmdTemp);
}



/*****************************************************************************************************
 * @name:_io_ScanWriteParamData
 * @brief:get actual value from value string according to parameter type.
 * @params:
 *     1.str:string of parameter value.
 *     2.ParamType:parameter type code, see "MACRO" part of c_communication.h
 *     3.pDataDst:data destination to be written in.
 * @retval: error code, -1 if read/write error, a integer greater than 0 if read/write succeed.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
int _io_ScanWriteParamData(char *str, uint8_t ParamType, uint8_t *pDataDst)
{
	int ErrorCode = EOF;


	/* there are some warnings here, but don't worry, they are correct and useful */
	switch(ParamType)
	{
	case PARAMTYPE_char:
	{
		ErrorCode = sscanf(str,"%c",(char*)pDataDst);
		break;
	}
	case PARAMTYPE_unsigned_char:
	{
		ErrorCode = sscanf(str,"%c",(unsigned char*)pDataDst);
		break;
	}
	case PARAMTYPE_int:
	{
		ErrorCode = sscanf(str,"%d",(int*)pDataDst);
		break;
	}
	case PARAMTYPE_unsigned_int:
	{
		ErrorCode = sscanf(str,"%d",(unsigned int*)pDataDst);
		break;
	}
	case PARAMTYPE_short:
	{
		ErrorCode = sscanf(str,"%d",(short*)pDataDst);
		break;
	}
	case PARAMTYPE_unsigned_short:
	{
		ErrorCode = sscanf(str,"%d",(unsigned short*)pDataDst);
		break;
	}
	case PARAMTYPE_long:
	{
		ErrorCode = sscanf(str,"%d",(long*)pDataDst);
		break;
	}
	case PARAMTYPE_unsigned_long:
	{
		ErrorCode = sscanf(str,"%d",(unsigned long*)pDataDst);
		break;
	}
	case PARAMTYPE_float:
	{
		ErrorCode = sscanf(str,"%f",(float*)pDataDst);
		break;
	}
	case PARAMTYPE_double:
	{
		ErrorCode = sscanf(str,"%f",(double*)pDataDst);
		break;
	}
	case PARAMTYPE_int8_t:
	{
		ErrorCode = sscanf(str,"%d",(int8_t*)pDataDst);
		break;
	}
	case PARAMTYPE_int16_t:
	{
		ErrorCode = sscanf(str,"%d",(int16_t*)pDataDst);
		break;
	}
	case PARAMTYPE_int32_t:
	{
		ErrorCode = sscanf(str,"%d",(int32_t*)pDataDst);
		break;
	}
	case PARAMTYPE_uint8_t:
	{
		ErrorCode = sscanf(str,"%d",(uint8_t*)pDataDst);
		break;
	}
	case PARAMTYPE_uint16_t:
	{
		ErrorCode = sscanf(str,"%d",(uint16_t*)pDataDst);
		break;
	}
	case PARAMTYPE_uint32_t:
	{
		ErrorCode = sscanf(str,"%d",(uint32_t*)pDataDst);
		break;
	}
	default:
		break;
	}
	return ErrorCode;
}



/*****************************************************************************************************
 * @name:io_CmdExecute
 * @brief: call callback function after command is found.
 * @params:
 *     1.Cmd:pointer of the found command
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void io_CmdExecute(COS_Cmd *Cmd)
{
	Cmd->CmdCallbackFunc(Cmd->ParamsContainer);
}


/*****************************************************************************************************
 * @name:_io_GetParamSizeByStr
 * @brief:recognize the string type from string and calculate it size.
 * @params:
 *     1.ParamStr:parameter string. e.g."int","float"
 * @retval: parameter type code, see "MACRO" part of c_communication.h
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_GetParamSizeByStr(char *ParamStr)
{
	uint8_t TypeCode;
	uint8_t Size;
	TypeCode = _io_GetParamType(ParamStr);
	Size = _io_GetParamSizeByTypeCode(TypeCode);
	return Size;
}



/*****************************************************************************************************
 * @name:_io_GetParamType
 * @brief:recognize the string type from string.
 * @params:
 *     1.Str:parameter string. e.g."int","float"
 * @retval: parameter type code, see "MACRO" part of c_communication.h
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_GetParamType(char *Str)
{
	if(Str==NULL)
	{
		return 0;
	}
	else if(!strcmp(Str,"uint8_t"))
	{
		return PARAMTYPE_uint8_t;
	}
	else if(!strcmp(Str,"uint16_t"))
	{
		return PARAMTYPE_uint16_t;
	}
	else if(!strcmp(Str,"uint32_t"))
	{
		return PARAMTYPE_uint32_t;
	}
	else if(!strcmp(Str,"float"))
	{
		return PARAMTYPE_float;
	}
	else if(!strcmp(Str,"double"))
	{
		return PARAMTYPE_double;
	}
	else if(!strcmp(Str,"int8_t"))
	{
		return PARAMTYPE_int8_t;
	}
	else if(!strcmp(Str,"int16_t"))
	{
		return PARAMTYPE_int16_t;
	}
	else if(!strcmp(Str,"int32_t"))
	{
		return PARAMTYPE_int32_t;
	}
	else if(!strcmp(Str,"unsigned char"))
	{
		return PARAMTYPE_unsigned_char;
	}
	else if(!strcmp(Str,"char"))
	{
		return PARAMTYPE_char;
	}
	else if(!strcmp(Str,"unsigned int"))
	{
		return PARAMTYPE_unsigned_int;
	}
	else if(!strcmp(Str,"int"))
	{
		return PARAMTYPE_int;
	}
	else if(!strcmp(Str,"unsigned short"))
	{
		return PARAMTYPE_unsigned_short;
	}
	else if(!strcmp(Str,"short"))
	{
		return PARAMTYPE_short;
	}
	else if(!strcmp(Str,"unsigned long"))
	{
		return PARAMTYPE_unsigned_long;
	}
	else if(!strcmp(Str,"long"))
	{
		return PARAMTYPE_long;
	}
	else
	{
		return 0;
	}


}





/*****************************************************************************************************
 * @name:_io_GetParamSizeByTypeCode
 * @brief:get parameter size by parameter type code.
 * @params:
 *     1.ParamType:parameter type code
 * @retval: size of parameter.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
uint8_t _io_GetParamSizeByTypeCode(uint8_t ParamType)
{
	if(ParamType==0)
	{
		return 0;
	}
	else if(ParamType==PARAMTYPE_uint8_t)
	{
		return sizeof(uint8_t);
	}
	else if(ParamType==PARAMTYPE_uint16_t)
	{
		return sizeof(uint16_t);
	}
	else if(ParamType==PARAMTYPE_uint32_t)
	{
		return sizeof(uint32_t);
	}
	else if(ParamType==PARAMTYPE_float)
	{
		return sizeof(float);
	}
	else if(ParamType==PARAMTYPE_double)
	{
		return sizeof(double);
	}
	else if(ParamType==PARAMTYPE_int8_t)
	{
		return sizeof(int8_t);
	}
	else if(ParamType==PARAMTYPE_int16_t)
	{
		return sizeof(int16_t);
	}
	else if(ParamType==PARAMTYPE_int32_t)
	{
		return sizeof(int32_t);
	}
	else if(ParamType==PARAMTYPE_unsigned_char)
	{
		return sizeof(unsigned char);
	}
	else if(ParamType==PARAMTYPE_char)
	{
		return sizeof(char);
	}
	else if(ParamType==PARAMTYPE_unsigned_int)
	{
		return sizeof(unsigned int);
	}
	else if(ParamType==PARAMTYPE_int)
	{
		return sizeof(int);
	}
	else if(ParamType==PARAMTYPE_unsigned_short)
	{
		return sizeof(unsigned short);
	}
	else if(ParamType==PARAMTYPE_short)
	{
		return sizeof(short);
	}
	else if(ParamType==PARAMTYPE_unsigned_long)
	{
		return sizeof(unsigned long);
	}
	else if(ParamType==PARAMTYPE_long)
	{
		return sizeof(long);
	}
	else
	{
		return 0;
	}

}
#ifdef __cplusplus
}
#endif

