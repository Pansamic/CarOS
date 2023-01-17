/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_controller.c
Author:      Pansamic
Version:     1.0
Create date: 2023.1.3
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v1.0 version.
*****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#include "./src/debug/c_debug.h"
#include "./src/motor/DCmotor/controller/c_controller.h"



/*****************************************************************************************************
 * @name:
 * @brief:
 * @params:
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void PID_Init(PID_t *PID_instance, float Kp, float Ki, float Kd)
{
	/* PID speed controller */
	COS_assert(Kp,warning,"found Kp=0 when initialize PID model.");
	COS_assert(Ki,warning,"found Ki=0 when initialize PID model.");
	COS_assert(Kd,warning,"found Kd=0 when initialize PID model.");

	PID_instance->Identifier = CONTROLLER_PID;

	PID_instance->BeforeLastVelocityBias = 0;
	PID_instance->CurrentVelocityBias = 0;
	PID_instance->LastVelocityBias = 0;

	PID_instance->FeedBack = 0;
	PID_instance->InputVal = 0;
	PID_instance->OutputVal = 0;

	PID_instance->Kp = Kp;
	PID_instance->Ki = Ki;
	PID_instance->Kd = Kd;
}



/*****************************************************************************************************
 * @name:
 * @brief:
 * @params:
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void PID_Update(void *PID_instance, AngVelocity_t Input, AngVelocity_t Feedback)
{
	PID_t *PID_Temp = (PID_t*)PID_instance;
	PID_Temp->CurrentVelocityBias = Input - Feedback;
	PID_Temp->OutputVal += ((PID_Temp->Kp) *(PID_Temp->CurrentVelocityBias-PID_Temp->LastVelocityBias))+
                           ((PID_Temp->Ki) *(PID_Temp->CurrentVelocityBias))+
                           ((PID_Temp->Kd) *(PID_Temp->CurrentVelocityBias-2*PID_Temp->LastVelocityBias+PID_Temp->BeforeLastVelocityBias));
	PID_Temp->BeforeLastVelocityBias = PID_Temp->LastVelocityBias;
	PID_Temp->LastVelocityBias = PID_Temp->CurrentVelocityBias;
}


/*****************************************************************************************************
 * @name:
 * @brief:
 * @params:
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void PID_GetOutVal(void *PID_instance, AngVelocity_t *OutputDst)
{
	*OutputDst = ((PID_t*)PID_instance)->OutputVal;
}






#ifdef __cplusplus
}
#endif
