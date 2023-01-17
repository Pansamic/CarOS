/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_app.c
Author:      Pansamic
Version:     0.1
Create date: 2023.1.2
Description: This file contains the definition of LoadApp().
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.2    create v0.1 version.
*****************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif
#include "./src/c_includes.h"
#include "./src/app/c_app.h"
#include "./src/debug/c_debug.h"


/*****************************************************************************************************
 * @name:LoadApp
 * @brief:Load application to specified io device from an application list.
 * @params:
 *     1.ioDevice:CarOS io device, usually '&cosio'.
 *     2.AppList:A 'COS_App[]' type of array that contains information of applications.
 *     3.AppAmount:The amount of applications in 'AppList'.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void LoadApp(COS_io *ioDevice, COS_App AppList[], uint8_t AppAmount)
{
	uint8_t i;
	for(i=0 ; i<AppAmount ; i++)
	{
		/* deem that an incorrect name is a memory read error */
		if(!((*AppList[i].Name>='a'&&*AppList[i].Name<='z') || (*AppList[i].Name>='A'&&*AppList[i].Name<='Z')))
		{
			COS_Logf(err,"read app list error:check 'AppAmount'");
			return ;
		}
		COS_Logf(info,"load app \"%s\"", AppList[i].Name);
		io_AddCmd(ioDevice, AppList[i].Name, AppList[i].ParamsDescription, AppList[i].CmdCallbackFunc);
	}
}













#ifdef __cplusplus
}
#endif
