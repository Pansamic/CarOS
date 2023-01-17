/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_4wheel_mecanum.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.3
Description: The header file that contains all of the information about 4-wheel
    mecanum car model. It contains some applications and driver functions.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v0.1 version.
*****************************************************************************/
#ifndef __C_4WHEEL_MECANUM_H_
#define __C_4WHEEL_MECANUM_H_
#ifdef _cplusplus
extern "C"{
#endif
#include "c_config.h"
#if (CAR_TYPE==FOUR_WHEEL_MECANUM)
#include "./src/motor/DCmotor/c_DCMotor.h"
#include "./src/app/c_app.h"


/**********************************************************************************************
 *                                                                                            *
 *                                         MACROS                                             *
 *                                                                                            *
 **********************************************************************************************/
#define CARAPPAMOUNT 6


/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/
typedef struct FourWheelMecanumModel CarType_t;

struct FourWheelMecanumModel
{
	DCMotor       *LeftFrontMotor;
	DCMotor       *LeftRearMotor;
	DCMotor       *RightFrontMotor;
	DCMotor       *RightRearMotor;

	float          WheelDiameter;            // unit:cm

	float         xAxisWheelDistance;
	float         yAxisWheelDistance;

	/* measured value */
	LinVelocity_t  CurrentXVelocity;
	LinVelocity_t  CurrentYVelocity;
	AngVelocity_t  CurrentAngularVelocity;

	/* corrected value */
	LinVelocity_t  AdjustedXVelocity;
	LinVelocity_t  AdjustedYVelocity;
	AngVelocity_t  AdjustedAngularVelocity;

	/* target value */
	LinVelocity_t  TargetXVelocity;
	LinVelocity_t  TargetYVelocity;
	AngVelocity_t  TargetAngularVelocity;

};
/**********************************************************************************************
 *                                                                                            *
 *                                         EXTERN                                             *
 *                                                                                            *
 **********************************************************************************************/
extern CarType_t Car;

extern DCMotor LeftFrontMotor;
extern DCMotor LeftRearMotor;
extern DCMotor RightFrontMotor;
extern DCMotor RightRearMotor;

extern COS_App CarAppList[CARAPPAMOUNT];


/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/
void    Car_Init               ( CarType_t *Car_instance, float WheelDiameter, float xAxisWheelDistance, float yAxisWheelDistance );
void    Car_AddWheel           ( DCMotor *LeftFront, DCMotor *LeftRear, DCMotor *RightFront, DCMotor *RightRear);
void    Car_SetVelocity        ( CarType_t *Car, Angle_t Angle, LinVelocity_t Speed, AngVelocity_t AngVelocity );
void    Car_AdjustedVelocity   ( CarType_t *Car );
void    Car_VelocityControl    ( CarType_t *Car );











#endif /* (CAR_TYPE==FOUR_WHEEL_MECANUM) */
#ifdef _cplusplus
}
#endif
#endif /* C_4WHEEL_MECANUM_H_ */
