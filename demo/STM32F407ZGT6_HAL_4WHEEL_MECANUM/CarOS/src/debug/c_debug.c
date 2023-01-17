#include <string.h>
#include <stdarg.h>
#include "usart.h"
#include "./src/debug/c_debug.h"
#include "./src/memory/c_memory.h"
#include "./src/communication/c_io.h"
/*****************************************************************************************
 *                                                                                       *
 *                                      VARIABLE                                         *
 *                                                                                       *
 *****************************************************************************************/
/* if enables log storage then enables log buffer,
 * otherwise disables log buffer */
#if (LOG_STORAGE==1)
char  COS_Log[1024*LOG_MEM_SIZE];
LogNode *LogChainHead;
#endif


/*****************************************************************************************
 *                                                                                       *
 *                                    DECLARATION                                        *
 *                                                                                       *
 *****************************************************************************************/
#if ((LOG_PRINT==1)||(LOG_STORAGE==1))
static uint8_t Log_Filter                 (const char *file, LogLevel level);
static char*   CutFileName                (const char* FileName);
#endif
#if LOG_STORAGE
static void    Log_PrintNode              (LogNode *LogNode);
void           COS_PrintLogByLevel        (LogLevel Level);
void           COS_PrintLogByFile         (const char *FileName);
void           COS_PrintLogByFunction     (const char *FuncName);
void           COS_PrintLogByString       (const char *str);
#endif /* LOG_STORAGE */
/*****************************************************************************************
 *                                                                                       *
 *                                     STRUCTURE                                         *
 *                                                                                       *
 *****************************************************************************************/



/*****************************************************************************************
 *                                                                                       *
 *                                       CONST                                           *
 *                                                                                       *
 *****************************************************************************************/
const char *LogLevelString[]=
{
	"EMERG",
	"ALERT",
	"CRIT",
	"ERROR",
	"WARN",
	"NOTICE",
	"INFO",
	"DEBUG"
};

/*****************************************************************************************
 *                                                                                       *
 *                                     FUNCTION                                          *
 *                                                                                       *
 *****************************************************************************************/
/*****************************************************************************************************
 * @name:COS_DebugInit
 * @brief:initialize debug system of CarOS system.
 * @params:
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_DebugInit()
{
#if LOG_STORAGE
	LogChainHead = NULL;
#endif
	COS_printf("debug init done.\r\n");
}



/*****************************************************************************************************
 * @name:_Log_Write
 * @brief:this is a log write function prototype, generating system log information string and
 *     formating user customized string with several variables.
 * @params:
 *     1.file:file name string,e.g."c_debug.h". Macro 'WriteLogf()' will gives this string.
 *     2.func:function name string, e.g."DCMotorSetSpeed". Macro 'WriteLogf()' will gives this string.
 *     3.line:current file line,e.g."34". Macro 'WriteLogf()' will gives this number.
 *     4.level:log level.
 *     5.fmt:user customized string with format sign such as "%s" and "%d".
 *     6.arguments:value to be put into 'fmt'.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void _Log_Write(const char *file,const char *func,const int line, LogLevel level, char *fmt,...)
{

#if (LOG_STORAGE==1)

	if(Log_Filter(file,level)==0)
	{
		return ;
	}
	va_list ap;                // argument list
	LogNode *NewLog;
	uint32_t LogStringLength;

	/* get current COS_Log string length so that function knows where it should
	 * start to write new log string. */
	LogStringLength = strlen(COS_Log);  // length of current COS_Log string

	/* allocate a block of memory to contain new log information */
	NewLog = (LogNode*)cos_malloc(sizeof(LogNode));

	if(NewLog==NULL) // allocate memory failed, exit function
	{
		return ;
	}

	NewLog->FileName = CutFileName(file);
	NewLog->FunctionName = func;
	NewLog->LogLevel = level;
	NewLog->RowNumber = line;
	NewLog->LogString = COS_Log+LogStringLength;
	NewLog->Next = NULL;

	va_start(ap,fmt);          // argument list initialize

	/* write customized string and format arguments behind system information which
	 * is just written in the message buffer before */
	vsprintf(COS_Log+LogStringLength,fmt,ap);

	/* deinit and clear argument list */
	va_end(ap);

	NewLog->LogStringLen = strlen(COS_Log+LogStringLength);

	/* add new node to log chain */
	LogNode *pTailNode = LogChainHead;
	if(pTailNode == NULL)
	{
		LogChainHead = NewLog;
	}
	else
	{
		while(pTailNode->Next != NULL)
		{
			pTailNode = pTailNode->Next;
		}
		pTailNode->Next = NewLog;
	}
#if LOG_PRINT==1
	COS_printf("%.*s\r\n",NewLog->LogStringLen, COS_Log+LogStringLength);
#endif /* LOG_PRINT==1 */
#elif (LOG_PRINT==1)

	if(Log_Filter(file,level)==0)
	{
		return ;
	}
	va_list ap;                // argument list
	va_start(ap,fmt);          // argument list initialize

	COS_printf("[%s][%s][%s][%d]",LogLevelString[level],CutFileName(file),func,line);

	/* write customized string and format arguments behind system information which
	 * is just written in the message buffer before */
	io_vprintf(&cosio,fmt,ap);

	va_end(ap);

	COS_printf("\r\n");

#endif
	return ;
}
/*****************************************************************************************************
 * @name:Log_Filter
 * @brief:This function filters out logs that should be or not be logged
 * @params:
 *     1.file:the string of file path and file name.
 *     2.level:integer.
 * @retval: 1 if the log should be logged ; 0 if the log shouldn't be logged.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
#if ((LOG_PRINT==1)||(LOG_STORAGE==1))
static uint8_t Log_Filter(const char *file, LogLevel level)
{
	/* Filter by level */
	switch(level)
	{
	case emerg:
	{
		if(LOG_EMERG==0)
		{
			return 0;
		}
		break;
	}
	case alert:
	{
		if(LOG_ALERT==0)
		{
			return 0;
		}
		break;
	}
	case crit:
	{
		if(LOG_CRIT==0)
		{
			return 0;
		}
		break;
	}
	case err:
	{
		if(LOG_ERR==0)
		{
			return 0;
		}
		break;
	}
	case warning:
	{
		if(LOG_WARNING==0)
		{
			return 0;
		}
		break;
	}
	case notice:
	{
		if(LOG_NOTICE==0)
		{
			return 0;
		}
		break;
	}
	case info:
	{
		if(LOG_INFO==0)
		{
			return 0;
		}
		break;
	}
	case debug:
	{
		if(LOG_DEBUG==0)
		{
			return 0;
		}
		break;
	}
	default:
		return 0;
	}/* end switch */

	/* Filter by file */
	if((DEBUG_CAR==0)&&(strstr(file,"car")))
	{
		return 0;
	}
	else if((DEBUG_DCMOTOR==0)&&(strstr(file,"DCmotor")))
	{
		return 0;
	}
	else if((DEBUG_ENCODER==0)&&(strstr(file,"encoder")))
	{
		return 0;
	}
	else if((DEBUG_L298N==0)&&(strstr(file,"L298N")))
	{
		return 0;
	}
	else if((DEBUG_A4950==0)&&(strstr(file,"A4950")))
	{
		return 0;
	}

	return 1;
}
#endif
#if LOG_STORAGE

/*****************************************************************************************************
 * @name:COS_PrintLog
 * @brief:cut part of file path of 'FileName'.
 * @params:
 *     1.FileName:file path and file name itself, such as "../User/CarOS/src/debug/c_debug.c"
 *     2.DataDst:the space to be written in.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static char* CutFileName(const char* FileName)
{
	char* pFileNameBeginning = NULL;

	pFileNameBeginning = strstr(FileName,"src/");

	if(pFileNameBeginning!=NULL)
	{
		pFileNameBeginning += 4;
	}
	else
	{
		pFileNameBeginning = FileName;
	}

	return pFileNameBeginning;
}
/*****************************************************************************************************
 * @name:COS_PrintLog
 * @brief:print all log
 * @params:none
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_PrintLog()
{
    LogNode *pNode = LogChainHead;    // a temporary pointer to operate LogChain.
	while(pNode!=NULL)
	{
		Log_PrintNode(pNode); // print log data to buffer
		pNode = pNode->Next;
	}
	return ;
}


/*****************************************************************************************************
 * @name:Log_PrintNode
 * @brief:generate complete log string to 'LogBufDst' and output current log string length to
 *     'LogMesgLength'.
 * @params:
 *     1.LogNode:pointer of struct LogNode.
 *     2.LogBufDst:The string pointer variable to be written with log information.
 *     3.LogMesgLength:length of current log string.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void Log_PrintNode(LogNode *LogNode)
{
    /* [WARN][c_DCMotor.c][DCMotor_SetSpeed][78] xxxxxxxxxx */
	/* Notice that COS_Log string doesn't store '\r' and '\n' */
    COS_printf("[%s][%s][%s][%d] %.*s\r\n",LogLevelString[LogNode->LogLevel]
                                                  ,LogNode->FileName
                                                  ,LogNode->FunctionName
                                                  ,LogNode->RowNumber
                                                  ,LogNode->LogStringLen
                                                  ,LogNode->LogString);

}



/*****************************************************************************************************
 * @name:COS_PrintLogByLevel
 * @brief:print all log whose level is 'Level'.
 * @params:
 *     1.Level:log level of logs to be printed.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_PrintLogByLevel(LogLevel Level)
{
    LogNode *pNode = LogChainHead;// a temporary pointer to operate LogChain.

	while(pNode!=NULL)
	{
		if(pNode->LogLevel == Level)
		{
			Log_PrintNode(pNode); // print log data to buffer
		}
		pNode = pNode->Next;
	}

}



/*****************************************************************************************************
 * @name:COS_PrintLogByFile
 * @brief:print all log whose log file name contains parameter string 'FileName'.
 * @params:
 *     1.FileName:file name of logs to be printed.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_PrintLogByFile(const char *FileName)
{
    LogNode *pNode = LogChainHead;// a temporary pointer to operate LogChain.

	while(pNode!=NULL)
	{
		if(strncmp(pNode->FileName, FileName, pNode->LogStringLen))
		{
			Log_PrintNode(pNode); // print log data to buffer
		}
		pNode = pNode->Next;
	}

}




/*****************************************************************************************************
 * @name:COS_PrintLogByFunction
 * @brief:print all log whose log function name contains parameter string 'FuncName'.
 * @params:
 *     1.FuncName:a constant string. The string
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_PrintLogByFunction(const char *FuncName)
{
    LogNode *pNode = LogChainHead;// a temporary pointer to operate LogChain.

	while(pNode!=NULL)
	{
		if(strncmp(pNode->FunctionName, FuncName, pNode->LogStringLen))
		{
			Log_PrintNode(pNode); // print log data to buffer
		}
		pNode = pNode->Next;
	}

}




/*****************************************************************************************************
 * @name:COS_PrintLogByString
 * @brief:print all log whose log string contains parameter string 'str'.
 * @params:
 *     1.str:the string you wanna compare with log string.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void COS_PrintLogByString(const char *str)
{
    LogNode *pNode = LogChainHead;// a temporary pointer to operate LogChain.

	while(pNode!=NULL)
	{
		if(strnstr(pNode->LogString,str,pNode->LogStringLen))
		{
			Log_PrintNode(pNode); // print log data to buffer
		}
		pNode = pNode->Next;
	}

}
#endif /* LOG_STORAGE */
