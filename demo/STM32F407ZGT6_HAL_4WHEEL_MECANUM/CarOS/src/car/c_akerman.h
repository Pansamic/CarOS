/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_akerman.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.3
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v0.1 version.
*****************************************************************************/
#ifndef __C_AKERMAN_H_
#define __C_AKERMAN_H_
#ifdef _cplusplus
extern "C"{
#endif
#include "c_config.h"
#if (CAR_TYPE==AKERMAN)
#include "./src/motor/DCmotor/c_DCMotor.h"
#include "./src/app/c_app.h"


/**********************************************************************************************
 *                                                                                            *
 *                                         MACROS                                             *
 *                                                                                            *
 **********************************************************************************************/
/* It must correspond to the actual quantity */
#define CARAPPAMOUNT 5


/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/
/* avoid cross include */
typedef struct CarDefinition CarType_t;


/**********************************************************************************************
 *                                                                                            *
 *                                         EXTERN                                             *
 *                                                                                            *
 **********************************************************************************************/
/* CarType_t instance declaration */
extern CarType_t Car;

/* Wheels declaration */
extern DCMotor LeftMotor;
extern DCMotor RightMotor;

/* app list based on car type */
extern COS_App CarAppList[CARAPPAMOUNT];

/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/










#endif /* (CAR_TYPE==AKERMAN) */
#ifdef _cplusplus
}
#endif
#endif /* __C_AKERMAN_H_ */
