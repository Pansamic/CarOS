/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_biltinapp.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.2
Description: The CarOS built-in applications. These applications generally
    fit for every car model and are system-level debug tools.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.2    create v0.1 version.
*****************************************************************************/
#ifndef __C_BUILTINAPP_H__
#define __C_BUILTINAPP_H__
#ifdef __cplusplus
extern "C"{
#endif
#include "./src/app/c_app.h"

#if LOG_STORAGE
#define BUILT_IN_APP_AMOUNT 2
#else
#define BUILT_IN_APP_AMOUNT 0
#endif
/**********************************************************************************************
 *                                                                                            *
 *                                         EXTERN                                             *
 *                                                                                            *
 **********************************************************************************************/
/* only provide a application list */
extern COS_App BuiltinAppList[BUILT_IN_APP_AMOUNT];














#ifdef __cplusplus
}
#endif
#endif
