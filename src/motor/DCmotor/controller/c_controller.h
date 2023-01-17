/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_controller.h
Author:      Pansamic
Version:     1.0
Create date: 2023.1.3
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v1.0 version.
*****************************************************************************/
#ifndef __C_CONTROLLER_H__
#define __C_CONTROLLER_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "./src/c_includes.h"

/**********************************************************************************************
 *                                                                                            *
 *                                         MACROS                                             *
 *                                                                                            *
 **********************************************************************************************/
#define CONTROLLER_NONE 0
#define CONTROLLER_PID  1


/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/
/********************************************************
 * @brief:
 * @params:
 *     1.Identifier:All controller have, used to identify which controller it is.
 *     2.Kp:
 * */
typedef struct PIDControllerDefinition
{
	uint8_t Identifier;
	float Kp;
	float Ki;
	float Kd;
	AngVelocity_t CurrentVelocityBias;
	AngVelocity_t LastVelocityBias;
	AngVelocity_t BeforeLastVelocityBias;
	AngVelocity_t InputVal;
	AngVelocity_t OutputVal;
	AngVelocity_t FeedBack;

}PID_t;


/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/
/* PID controller related */
void        PID_Init                    ( PID_t *PID_instance, float Kp, float Ki, float Kd);
void        PID_Update                  ( void *PID_instance, AngVelocity_t Input, AngVelocity_t Feedback);
void        PID_GetOutVal               ( void *PID_instance, AngVelocity_t *OutputDst );




#ifdef __cplusplus
}
#endif
#endif /* __C_CONTROLLER_H__ */
