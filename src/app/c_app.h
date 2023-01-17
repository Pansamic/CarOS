/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_app.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.2
Description: This file describes an simple application model of CarOS and a
    function that loads apps to specified io devicce.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.2    create v0.1 version.
*****************************************************************************/
#ifndef __C_APP_H__
#define __C_APP_H__
#ifdef __cplusplus
extern "C"{
#endif
#include "./src/communication/c_io.h"


/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/


/**********************************************************************************************
 * @brief: The description of CarOS application.
 * @params:
 *     1.Name:The name of this application.
 *     2.ParamsDescription:A string that describes the parameters the application need. Each
 *         parameter is divided ONLY by ','! e.g. "int,float,uint32_t"
 *     3.CmdCallbackFunc:A pointer of function.
 * */
typedef struct AppDefinition
{
	const char  *Name;
	const char  *ParamsDescription;
	void       (*CmdCallbackFunc)(uint8_t*);
}COS_App;


/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/
void LoadApp(COS_io *ioDevice, COS_App AppList[], uint8_t AppAmount);




#ifdef __cplusplus
}
#endif
#endif
