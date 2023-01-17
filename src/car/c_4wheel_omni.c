/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_4wheel_omni.c
Author:      Pansamic
Version:     0.1
Create date: 2023.1.3
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v0.1 version.
*****************************************************************************/
#ifdef _cplusplus
extern "C"{
#endif
#include "c_config.h"
#if (CAR_TYPE==FOUR_WHEEL_OMNI)
#include <math.h>
#include "./src/c_types.h"
#include "./src/debug/c_debug.h"
#include "./src/car/c_car.h"
#include "./src/car/c_4wheel_omni.h"


COS_App CarAppList[CARAPPAMOUNT]={

};

CarType_t Car;

DCMotor FrontMotor;
DCMotor BehindMotor;
DCMotor LeftMotor;
DCMotor RightMotor;


/*****************************************************************************************************
 * @name:
 * @brief:
 * @params:
 * @retval:none
 * @author:
 *****************************************************************************************************/









#endif /* CAR_TYPE==FOUR_WHEEL_OMNI */
#ifdef _cplusplus
}
#endif
