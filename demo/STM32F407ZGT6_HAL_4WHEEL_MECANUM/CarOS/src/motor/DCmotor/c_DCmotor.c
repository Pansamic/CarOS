/*****************************************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_DCmotor.h
Author:      Pansamic
Version:     0.1
Create date: 2022.12.10
Description: 
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2022.12.10    create v0.1 version.
*****************************************************************************/
#include <stdlib.h>
#include "./src/debug/c_debug.h"
#include "./src/memory/c_memory.h"
#include "./src/motor/DCmotor/c_DCmotor.h"
#include "./src/motor/DCmotor/driver/DCMotor_driver.h"
#include "./src/motor/DCmotor/controller/c_controller.h"
/****************************************************
 * @brief: This is a macro to transform realistic
 *     speed to pwm generating timer's counter compare
 *     value.
 * */
#define VelocityToTimerCounterPeriod(Motor,VelocityVal) ((VelocityVal)/(Motor->MaxVelocity))*(Motor->PWMCounterPeriod)



static void DCMotorVelocityControl(DCMotor *Motor);


/*****************************************************************************************************
 * @name:DCMotorInit
 * @brief:This function acts as a 'DCMotor' instance variables initializer and parameters checker. The
 *     function set some speed-related variables to zero to avoid unsure value of variables. It also
 *     checks whether user puts wrong value or even nothing to the 'DCMotor' model.
 * @params:
 *     1.Motor:A pointer to 'DCMotor' instance.
 * @retval:CarOS error code. Please check 'debug' instruction. More detail of error code is in
 *     'CarOS/debug/c_debug.h' and 'CarOS/debug/c_debug.c' documents.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void DCMotor_Init( DCMotor *Motor,const char *Name,AngVelocity_t MaxAngVelocity,uint8_t RotateDirectionReverse,uint32_t PWMCounterPeriod)
{
	if(Motor==NULL)
	{
		COS_Logf(err,"passing null dc motor pointer to function, function exit.");
		return ;
	}

	/* check whether name is legal */
	if(!(((*(Name)>='a')&&(*(Name)<='z'))||((*(Name)>='A')&&(*(Name)<='Z'))))
	{
		COS_Logf(notice,"motor has illegal name, now rename it \"Nameless Motor\"");
		Motor->Name = "Nameless Motor";
	}
	else
	{
		Motor->Name = Name;
	}
	COS_assert(MaxAngVelocity,warning,"max speed of %s is zero.",Motor->Name);
	Motor->MaxVelocity = MaxAngVelocity;

	Motor->RotateDirectionReverse = RotateDirectionReverse;

	Motor->PWMCounterPeriod = PWMCounterPeriod;

	/* Set all Velocity related variables to zero, in avoid of unspecified */
	Motor->AdjustedVelocity = 0;
	Motor->CurrentVelocity = 0;
	Motor->TargetVelocity = 0;

	Motor->EncoderEnable = 0;

	COS_Logf(info,"%s initialization done.",Motor->Name);
}


/*****************************************************************************************************
 * @name:
 * @brief:
 * @params:
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
DCMotor* DCMotor_Create(const char * Name)
{
	DCMotor *NewMotor;
	NewMotor = (DCMotor*)cos_malloc(sizeof(DCMotor));
	if(NewMotor==NULL)
	{
		COS_Logf(warning,"%s create fail.",Name);
	}
	else
	{
		COS_Logf(info,"%s create success.",Name);
		NewMotor->Name = Name;
	}
	return NewMotor;
}



/*****************************************************************************************************
 * @name:DCMotor_SetVelocity
 * @brief: ONLY seperately SET the speed of one motor, but not update motor speed. If you want to
 *     update motor speed, you have to call 'DCMotorAdjustVelocity' function periodically.
 * @params:
 *     1.Motor:A pointer to 'DCMotor' instance.
 *     2.VelocityVal: speed, unit:cm/s.
 * @retval:CarOS error code. Please check 'debug' instruction. More detail of error code is in
 *     'CarOS/debug/c_debug.h' and 'CarOS/debug/c_debug.c' documents.
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void DCMotor_SetVelocity(DCMotor* Motor, AngVelocity_t VelocityVal)
{
	/* check whether variables is correctly passing to this function */
	if(Motor==NULL)
	{
		COS_Logf(err,"passing null dc motor pointer to function, function exit.");
		return ;
	}

	/* constrain speed value */
	if(VelocityVal>Motor->MaxVelocity)
	{
		Motor->TargetVelocity = Motor->MaxVelocity;
	}
	else if(VelocityVal<(-Motor->MaxVelocity))
	{
		Motor->TargetVelocity = -Motor->MaxVelocity;
	}
	else
	{
		Motor->TargetVelocity = VelocityVal;
	}
	COS_Logf(debug,"%s:set speed to %f",Motor->Name, VelocityVal);
}


/*****************************************************************************************************
 * @name:DCMotor_AdjustVelocity
 * @brief: update DC motor speed and apply the adjusted speed to DC motor driver.
 * @params:
 *     1.Motor:A pointer to 'DCMotor' instance.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void DCMotor_AdjustVelocity(DCMotor* Motor)
{
	int32_t TimerCounterLoadVal;
	DCMotorVelocityControl(Motor);
	TimerCounterLoadVal = VelocityToTimerCounterPeriod(Motor,(Motor->AdjustedVelocity));

	if(Motor->RotateDirectionReverse==1)
	{
		Motor->DriverSetVelocitycb(Motor->Driver, -TimerCounterLoadVal);
	}
	else
	{
		Motor->DriverSetVelocitycb(Motor->Driver, TimerCounterLoadVal);
	}
}

/*****************************************************************************************************
 * @name:DCMotorVelocityControl
 * @brief:This function changes 'AdjustedVelocity' attribute of 'DCMotor' model. 'AdjustedVelocity' attribute
 *     is actually the attribute related to PWM compare value registers directly, because PWM compare
 *     value is calculated by 'AdjustedVelocity'.
 *     This function only changes 'AdjustedVelocity' and let 'DCMotorAdjustVelocity()' function to execute
 *     PWM compare value register.
 * @params:
 *     1.Motor:A pointer of 'DCMotor' instance.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void DCMotorVelocityControl(DCMotor *Motor)
{
	/* if this motor doesn't use encoder, then directly set its velocity to
	 * Target Velocity without PID control */
	if(Motor->EncoderEnable==0)
	{
		Motor->AdjustedVelocity = Motor->TargetVelocity;
		COS_Logf(debug,"%s adjust velocity without encoder.",Motor->Name);
		return ;
	}

	/* update encoder count number of encoder model, actually read compare counts of timer */
	EncoderUpdate((&(Motor->Encoder)));

	/* velocity control algorithm will calculate a proper value and store it in DCMotor model */
	Motor->CurrentVelocity = (AngVelocity_t)
			                 (((AngVelocity_t)Motor->Encoder.CurrentCount)*6.2831853f)/
						     (((AngVelocity_t)Motor->Encoder.PulsePerRound)*
	                         ((AngVelocity_t)Motor->Encoder.Interval/1000));

	Motor->ControllerUpdate(Motor->VelocityController, Motor->TargetVelocity, Motor->CurrentVelocity);
	Motor->ControllerGetOutVal(Motor->VelocityController, &Motor->AdjustedVelocity);

	/* constrain the value of velocity */
	if(Motor->AdjustedVelocity>Motor->MaxVelocity)
	{
		Motor->AdjustedVelocity=Motor->MaxVelocity;
	}
	else if(Motor->AdjustedVelocity<(-Motor->MaxVelocity))
	{
		Motor->AdjustedVelocity=-Motor->MaxVelocity;
	}

}


/*****************************************************************************************************
 * @name:DCMotor_AddPID
 * @brief:add PID controller to dc motor model.
 * @params:
 *     1.Motor:pointer of struct 'DCMotor'.
 *     2.Kp:proportional parameter of PID model.
 *     3.Ki:integral parameter of PID model.
 *     4.Kd:differential parameter of PID model.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void DCMotor_AddPID(DCMotor *Motor, float Kp, float Ki, float Kd)
{
	PID_t *NewPID = (PID_t*)cos_malloc(sizeof(PID_t));

	if(NewPID==NULL)
	{
		COS_Logf(err,"%s add PID velocity controller failed, please check memory size.",Motor->Name);
		return ;
	}

	Motor->VelocityController = NewPID;


	PID_Init(NewPID, Kp, Ki, Kd);
	Motor->ControllerUpdate = PID_Update;
	Motor->ControllerGetOutVal = PID_GetOutVal;
	COS_Logf(info,"%s adding PID velocity controller succeed.",Motor->Name);
}

/*****************************************************************************************************
 * @name:DCMotor_AddEncoder
 * @brief:add an encoder to dc motor model and initialize the encoder.
 * @params:
 *     1.Motor:pointer of struct 'DCMotor'.
 *     2.htim:pointer of usart insatnce.
 *     3.PulsePerRound:how many times does encoder record after motor rotate a round.
 *     4.Interval:the time period of reading encoder value. It's significant to the velocity calculation.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void DCMotor_AddEncoder(DCMotor *Motor, TIM_HandleTypeDef *htim, uint16_t PulsePerRound, uint16_t Interval)
{
	if(Motor==NULL)
	{
		COS_Logf(err,"add encoder failed:passing null motor.");
		return ;
	}

	COS_assert(htim,warning,"passing null timer when add encoder.");

	/* See whether the interval used to calculate speed isn't set to correct value */
	COS_assert(Interval,warning,"encoder interval of is zero.");

	/* See whether quantity of pulses per round is correctly set */
	COS_assert(PulsePerRound,warning,"pulse-per-round of encoder is zero.",Motor->Name);

	EncoderInit(&Motor->Encoder, htim, PulsePerRound, Interval);
	Motor->EncoderEnable = 1;

}

