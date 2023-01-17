#ifndef __C_DEBUG_H_
#define __C_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "c_config.h"

/*****************************************************************************************
 *                                                                                       *
 *                                      MACROS                                           *
 *                                                                                       *
 *****************************************************************************************/
#define COS_assert(a,level,format, arg...) if((a)==0)_Log_Write( __FILE__,__FUNCTION__, __LINE__, level, format, ##arg)
#define COS_Logf(level,format, arg...) _Log_Write( __FILE__,__FUNCTION__, __LINE__, level, format, ##arg)

/*****************************************************************************************
 *                                                                                       *
 *                                      TYPEDEF                                          *
 *                                                                                       *
 *****************************************************************************************/
typedef struct LogNodeDefinition
{
	uint8_t LogLevel;                // log level, ranging from 0 to 7
	const char *FileName;           // the name of file where log macro resides
	const char *FunctionName;       // the name of the function where log macro resides
 	uint16_t RowNumber;             // row number of log macro
	char *LogString;               // log information string
	int LogStringLen;              // LogStringLen locates the string
	struct LogNodeDefinition *Next;// next log chain node

}LogNode;

typedef enum LogLevelDefinition
{
	emerg,      // emergency circumstance, system is unusable(system crash)
	alert,      // immediate repair operation required
	crit,       // critical conditions, may influence part of functionality of program
	err,        // normal error
	warning,    // normal warning
	notice,     // not error, but need to handle
	info,       // general system information
	debug       // system-level information, very detailed
}LogLevel;

#if (LOG_STORAGE==1)
extern char COS_Log[1024*LOG_MEM_SIZE];
extern LogNode *LogChainHead;
#endif


/*****************************************************************************************
 *                                                                                       *
 *                                     FUNCTION                                          *
 *                                                                                       *
 *****************************************************************************************/
void COS_DebugInit();
void _Log_Write(const char *file,const char *func,const int line, LogLevel level, char *fmt,...);

#if LOG_STORAGE

void COS_PrintLog();

#endif /* LOG_STORAGE */





/**********************************/
/*      Default Settings          */
/*       DON'T Modify             */
/**********************************/
#ifndef COS_DEBUG
#define COS_DEBUG    1
#endif

#ifndef DEBUG_CAR
#define DEBUG_CAR     1
#endif

#ifndef DEBUG_DCMOTOR
#define DEBUG_DCMOTOR 1
#endif

#ifndef DEBUG_ENCODER
#define DEBUG_ENCODER 1
#endif

#ifndef DEBUG_L298N
#define DEBUG_L298N  1
#endif

#ifndef DEBUG_A4950
#define DEBUG_A4950  1
#endif

#ifndef LOG_PRINT
#define LOG_PRINT    1
#endif

#ifndef LOG_STORAGE
#define LOG_STORAGE  1
#endif

#ifndef LOG_EMERG
#define LOG_EMERG    1
#endif

#ifndef LOG_ALERT
#define LOG_ALERT    1
#endif

#ifndef LOG_CRIT
#define LOG_CRIT     1
#endif

#ifndef LOG_ERR
#define LOG_ERR      1
#endif

#ifndef LOG_WARNING
#define LOG_WARNING  1
#endif

#ifndef LOG_NOTICE
#define LOG_NOTICE   1
#endif

#ifndef LOG_INFO
#define LOG_INFO     1
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG    0
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif

