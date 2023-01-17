/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_4wheel_mecanum.h
Author:      Pansamic
Version:     0.1
Create date: 2023.1.3
Description: This file contains realization of driver functions and applications
    of 4-wheel mecanum car model.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v0.1 version.
*****************************************************************************/
#ifdef _cplusplus
extern "C"{
#endif
#include "c_config.h"
#if (CAR_TYPE==FOUR_WHEEL_MECANUM)
#include <math.h>
#include "./src/c_types.h"
#include "./src/debug/c_debug.h"
#include "./src/car/c_car.h"
#include "./src/car/c_4wheel_mecanum.h"

#include "usart.h"
/* command callback function */
void SetVelocitycb        (uint8_t* ParamList);
void ShowPIDcb            (uint8_t* ParamList);
void SetMotorPIDcb        (uint8_t* ParamList);
void SetCarVelocitycb     (uint8_t* ParamList);
void MotorPIDSelfAdjustcb (uint8_t* ParamList);


COS_App CarAppList[CARAPPAMOUNT]={
		/* set motor velocity */
		{"smv", "float,float,float,float", SetVelocitycb},

		/* show motor PID */
		{"shmpid", NULL, ShowPIDcb},

		/* set motor pid */
		{"smpid", "uint8_t,float,float,float", SetMotorPIDcb},

		/* set car velocity, notice that unit of angle is degree rather than radian */
		{"scv", "uint16_t,float,float", SetCarVelocitycb},

		/* motor PID self adjust */
		{"mpidsa", NULL, MotorPIDSelfAdjustcb}
};

CarType_t Car;

DCMotor LeftFrontMotor;
DCMotor LeftRearMotor;
DCMotor RightFrontMotor;
DCMotor RightRearMotor;


/*****************************************************************************************************
 * @name:Car_Init
 * @brief:Set all parameters related to kinemactics to zero and load some neccessary parameters to
 *     4-wheel mecanum car model.
 * @params:
 *     1.Car_instance:A pointer of car instance.
 *     2.WheelDiameter:The diameter of wheel.
 *     3.xAxisWheelDistance:The distance between the center of the front and rear wheels.
 *     4.yAxisWheelDistance:The distance between the center of the left and right wheels.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void Car_Init(CarType_t *Car_instance, float WheelDiameter, float xAxisWheelDistance, float yAxisWheelDistance)
{
	/* set the motor pointer as NULL to avoid wild pointer.
	 * when function 'Car_AddWheel()' is called, the pointer
	 * will be correct value. Many of functions check whether
	 * DC motor is added by examining whether the pointer is
	 * NULL or not. */
	Car_instance->LeftFrontMotor = NULL;
	Car_instance->LeftRearMotor  = NULL;
	Car_instance->RightFrontMotor= NULL;
	Car_instance->RightRearMotor = NULL;

	Car_instance->WheelDiameter = WheelDiameter;

	Car_instance->xAxisWheelDistance = xAxisWheelDistance;
	Car_instance->yAxisWheelDistance = yAxisWheelDistance;

	/* All kinematic-related parameters are set to zero to avoid
	 * uninitialized random values affecting the initial state
	 * of the kinematic model. */
	Car_instance->CurrentXVelocity        = 0;
	Car_instance->CurrentYVelocity        = 0;
	Car_instance->CurrentAngularVelocity  = 0;

	Car_instance->AdjustedXVelocity       = 0;
	Car_instance->AdjustedYVelocity       = 0;
	Car_instance->AdjustedAngularVelocity = 0;

	Car_instance->TargetXVelocity         = 0;
	Car_instance->TargetYVelocity         = 0;
	Car_instance->TargetAngularVelocity   = 0;
}
/*****************************************************************************************************
 * @name:Car_AddWheel
 * @brief:set pointer of motor as the pointer of 'DCMotor' type of struct.
 * @params:
 *     1.LeftFront:The pointer of left front 'DCMotor' struct.
 *     2.LeftRear:The pointer of left rear 'DCMotor' struct.
 *     3.RightFront:The pointer of right front 'DCMotor' struct.
 *     4.RightRear:The pointer of right rear 'DCMotor' struct.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void Car_AddWheel(DCMotor *LeftFront, DCMotor *LeftRear, DCMotor *RightFront, DCMotor *RightRear)
{

	COS_assert(LeftFront, warning, "car add Left Front Motor failed:passing null motor.");
	COS_assert(LeftRear,  warning, "car add Left Front Motor failed:passing null motor.");
	COS_assert(RightFront,warning, "car add Left Front Motor failed:passing null motor.");
	COS_assert(RightRear, warning, "car add Left Front Motor failed:passing null motor.");

	Car.LeftFrontMotor  = LeftFront;
	Car.LeftRearMotor   = LeftRear;
	Car.RightFrontMotor = RightFront;
	Car.RightRearMotor  = RightRear;

}
/*****************************************************************************************************
 * @name:Car_SetVelocity
 * @brief:set target velocity of car to input value.
 * @params:
 *     1.Car:Pointer of 'CarType_t' instance.
 *     2.Angle:Move towards the direction of 'Angle' counterclockwise with the front as the reference.
 *     3.Velocity:Linear velocity of car.
 *     4.AngVelocity:Angular velocity of car.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void Car_SetVelocity(CarType_t *Car, Angle_t Angle, LinVelocity_t Velocity, AngVelocity_t AngVelocity)
{
	Car->TargetYVelocity = Velocity * sinf(Angle);
	Car->TargetXVelocity = Velocity * cosf(Angle);
	Car->TargetAngularVelocity = AngVelocity;
	COS_Logf(info,"car set move towards %.2fdegree at speed %.2fcm/s, at angular velocity %.2frad/s.",Angle*180/3.141593,Velocity,AngVelocity);
}

/*****************************************************************************************************
 * @name:Car_AdjustedVelocity
 * @brief:Adjust attitude and apply velocity to wheels.
 * @params:
 *     1.Car:Pointer of 'CarType_t' instance.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void Car_AdjustedVelocity(CarType_t *Car)
{
	/* caution:this is the velocity of wheel ranther than motor,
	 * so its unit is cm/s rather than rad/s */
	AngVelocity_t LeftFrontWheelAngularVelocity   = 0;  // unit:rad/s
	AngVelocity_t LeftRearWheelAngularVelocity    = 0;  // unit:rad/s
	AngVelocity_t RightFrontWheelAngularVelocity  = 0;  // unit:rad/s
	AngVelocity_t RightRearWheelAngularVelocity   = 0;  // unit:rad/s
	LinVelocity_t LeftFrontWheelVelocity          = 0;  // unit:cm/s
	LinVelocity_t LeftRearWheelVelocity           = 0;  // unit:cm/s
	LinVelocity_t RightFrontWheelVelocity         = 0;  // unit:cm/s
	LinVelocity_t RightRearWheelVelocity          = 0;  // unit:cm/s
	LinVelocity_t xAxisVelocity                   = 0;  // unit:cm/s
	LinVelocity_t yAxisVelocity                   = 0;  // unit:cm/s

	/* closed-loop control */
	Car_VelocityControl(Car);

	xAxisVelocity = Car->AdjustedXVelocity;
	yAxisVelocity = Car->AdjustedYVelocity;

	LeftFrontWheelVelocity  = xAxisVelocity + yAxisVelocity - Car->AdjustedAngularVelocity * (Car->yAxisWheelDistance + Car->xAxisWheelDistance);
	LeftRearWheelVelocity   = xAxisVelocity - yAxisVelocity - Car->AdjustedAngularVelocity * (Car->yAxisWheelDistance + Car->xAxisWheelDistance);
	RightFrontWheelVelocity = xAxisVelocity - yAxisVelocity + Car->AdjustedAngularVelocity * (Car->yAxisWheelDistance + Car->xAxisWheelDistance);
	RightRearWheelVelocity  = xAxisVelocity + yAxisVelocity + Car->AdjustedAngularVelocity * (Car->yAxisWheelDistance + Car->xAxisWheelDistance);

	LeftFrontWheelAngularVelocity  = LinVelToAngVel(Car->WheelDiameter/2,LeftFrontWheelVelocity);
	LeftRearWheelAngularVelocity   = LinVelToAngVel(Car->WheelDiameter/2,LeftRearWheelVelocity );
	RightFrontWheelAngularVelocity = LinVelToAngVel(Car->WheelDiameter/2,RightFrontWheelVelocity);
	RightRearWheelAngularVelocity  = LinVelToAngVel(Car->WheelDiameter/2,RightRearWheelVelocity);

	DCMotor_SetVelocity(Car->LeftFrontMotor, LeftFrontWheelAngularVelocity );
	DCMotor_SetVelocity(Car->LeftRearMotor,  LeftRearWheelAngularVelocity  );
	DCMotor_SetVelocity(Car->RightFrontMotor,RightFrontWheelAngularVelocity);
	DCMotor_SetVelocity(Car->RightRearMotor, RightRearWheelAngularVelocity );

	DCMotor_AdjustVelocity(Car->LeftFrontMotor );
	DCMotor_AdjustVelocity(Car->LeftRearMotor  );
	DCMotor_AdjustVelocity(Car->RightFrontMotor);
	DCMotor_AdjustVelocity(Car->RightRearMotor );

}

/*****************************************************************************************************
 * @name:Car_VelocityControl
 * @brief:Adjust the attitude and motion with attitude sensor.
 * @params:
 *     1.Car:Pointer of 'CarType_t' instance.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void Car_VelocityControl(CarType_t *Car)
{
	/* The driver for MPU has not yet been developed yet,
	 * so velocity control has nothing to do */
	Car->AdjustedAngularVelocity = Car->TargetAngularVelocity;
	Car->AdjustedXVelocity       = Car->TargetXVelocity;
	Car->AdjustedYVelocity       = Car->TargetYVelocity;
}




/*****************************************************************************************************
 * @name:SetVelocitycb
 * @brief:callback function of application 'smv'
 * @params:
 *     1.ParamList:The parameters' data passed from command processor.
 *     1.1.VelocityVal1:Left  front wheel velocity. type:float ; unit:rad/s.
 *     1.2.VelocityVal2:Left  rear  wheel velocity. type:float ; unit:rad/s.
 *     1.3.VelocityVal3:Right front wheel velocity. type:float ; unit:rad/s.
 *     1.4.VelocityVal4:Right rear  wheel velocity. type:float ; unit:rad/s.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void SetVelocitycb(uint8_t* ParamList)
{
	uint8_t SizeSum = 0;
	float LeftFrontVelocity = *(float*)(ParamList + SizeSum);
	SizeSum += sizeof(float);
	float LeftRearVelocity = *(float*)(ParamList + SizeSum);
	SizeSum += sizeof(float);
	float RightFrontVelocity = *(float*)(ParamList + SizeSum);
	SizeSum += sizeof(float);
	float RightRearVelocity = *(float*)(ParamList + SizeSum);

	float xAxisVelocity = (LeftFrontVelocity+RightFrontVelocity)/2;
	float yAxisVelocity = (LeftRearVelocity+RightRearVelocity)/2;
	float AngVelocity = (RightFrontVelocity+RightRearVelocity-LeftFrontVelocity-LeftRearVelocity)/(4*(Car.xAxisWheelDistance+Car.yAxisWheelDistance));

	Car.TargetXVelocity = xAxisVelocity;
	Car.TargetYVelocity = yAxisVelocity;
	Car.TargetAngularVelocity = AngVelocity;

    COS_Logf(info,"set motor velocity: LF:%.2f ; LR:%.2f ; RF:%.2f ; RR:%.2f",LeftFrontVelocity,
                                                                              LeftRearVelocity,
                                                                              RightFrontVelocity,
                                                                              RightRearVelocity);
}



/*****************************************************************************************************
 * @name:ShowPIDcb
 * @brief:show PID value of each wheel. This command doesn't need parameter.
 * @params:
 *     1.ParamList:The parameters' data passed from command processor.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void ShowPIDcb(uint8_t* ParamList)
{
	if(((PID_t*)(Car.LeftFrontMotor->VelocityController))->Identifier!=CONTROLLER_PID)
	{
		COS_printf("%s doesn't have PID controller.\r\n",Car.LeftFrontMotor->Name);
	}
	else
	{
		COS_printf("LeftFront   Kp:%-6.3f\r\n",((PID_t*)(Car.LeftFrontMotor->VelocityController))->Kp);
		COS_printf("LeftFront   Ki:%-6.3f\r\n",((PID_t*)(Car.LeftFrontMotor->VelocityController))->Ki);
		COS_printf("LeftFront   Kd:%-6.3f\r\n",((PID_t*)(Car.LeftFrontMotor->VelocityController))->Kd);
	}
	if(((PID_t*)(Car.LeftRearMotor->VelocityController))->Identifier!=CONTROLLER_PID)
	{
		COS_printf("%s doesn't have PID controller.\r\n",Car.LeftRearMotor->Name);
	}
	else
	{
		COS_printf("LeftRear    Kp:%-6.3f\r\n",((PID_t*)(Car.LeftRearMotor->VelocityController))->Kp);
		COS_printf("LeftRear    Ki:%-6.3f\r\n",((PID_t*)(Car.LeftRearMotor->VelocityController))->Ki);
		COS_printf("LeftRear    Kd:%-6.3f\r\n",((PID_t*)(Car.LeftRearMotor->VelocityController))->Kd);
	}
	if(((PID_t*)(Car.RightFrontMotor->VelocityController))->Identifier!=CONTROLLER_PID)
	{
		COS_printf("%s doesn't have PID controller.\r\n",Car.RightFrontMotor->Name);
	}
	else
	{
		COS_printf("RightFront  Kp:%-6.3f\r\n",((PID_t*)(Car.RightFrontMotor->VelocityController))->Kp);
		COS_printf("RightFront  Ki:%-6.3f\r\n",((PID_t*)(Car.RightFrontMotor->VelocityController))->Ki);
		COS_printf("RightFront  Kd:%-6.3f\r\n",((PID_t*)(Car.RightFrontMotor->VelocityController))->Kd);
	}
	if(((PID_t*)(Car.RightRearMotor->VelocityController))->Identifier!=CONTROLLER_PID)
	{
		COS_printf("%s doesn't have PID controller.\r\n",Car.RightRearMotor->Name);
	}
	else
	{
		COS_printf("RightRear   Kp:%-6.3f\r\n",((PID_t*)(Car.RightRearMotor->VelocityController))->Kp);
		COS_printf("RightRear   Ki:%-6.3f\r\n",((PID_t*)(Car.RightRearMotor->VelocityController))->Ki);
		COS_printf("RightRear   Kd:%-6.3f\r\n",((PID_t*)(Car.RightRearMotor->VelocityController))->Kd);
	}
}



/*****************************************************************************************************
 * @name:SetMotorPIDcb
 * @brief:set PID parameters of specified motor.
 * @params:
 *     1.ParamList:The parameters' data passed from command processor.
 *     1.1.MotorIndex:0-left front ; 1-left rear ; 2-right front ; 3-right rear
 *     1.2.Kp:proportion parameter.
 *     1.3.Ki:integrate parameter.
 *     1.4.Kd:differential parameter.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void SetMotorPIDcb(uint8_t* ParamList)
{

	uint8_t SizeSum    = 0;
	uint8_t MotorIndex = 0;
	float   Kp         = 0;
	float   Ki         = 0;
	float   Kd         = 0;

	MotorIndex = *(uint8_t*)(ParamList + SizeSum);
	SizeSum   += sizeof(uint8_t);
	Kp         = *(float*)  (ParamList + SizeSum);
	SizeSum   += sizeof(float);
	Ki         = *(float*)  (ParamList + SizeSum);
	SizeSum   += sizeof(float);
	Kd         = *(float*)  (ParamList + SizeSum);

	switch(MotorIndex)
	{
	case 0:
	{
		((PID_t*)(Car.LeftFrontMotor->VelocityController))->Kp = Kp;
		((PID_t*)(Car.LeftFrontMotor->VelocityController))->Ki = Ki;
		((PID_t*)(Car.LeftFrontMotor->VelocityController))->Kd = Kd;
		COS_Logf(info,"%s has set pid Kp=%.2f Ki=%.2f Kd=%.2f.",Car.LeftFrontMotor->Name,
				((PID_t*)(Car.LeftFrontMotor->VelocityController))->Kp,
				((PID_t*)(Car.LeftFrontMotor->VelocityController))->Ki,
				((PID_t*)(Car.LeftFrontMotor->VelocityController))->Kd);
		break;
	}
	case 1:
	{
		((PID_t*)(Car.LeftRearMotor->VelocityController))->Kp = Kp;
		((PID_t*)(Car.LeftRearMotor->VelocityController))->Ki = Ki;
		((PID_t*)(Car.LeftRearMotor->VelocityController))->Kd = Kd;
		COS_Logf(info,"%s has set pid Kp=%.2f Ki=%.2f Kd=%.2f.",Car.LeftRearMotor->Name,
				((PID_t*)(Car.LeftRearMotor->VelocityController))->Kp,
				((PID_t*)(Car.LeftRearMotor->VelocityController))->Ki,
				((PID_t*)(Car.LeftRearMotor->VelocityController))->Kd);
		break;
	}
	case 2:
	{
		((PID_t*)(Car.RightFrontMotor->VelocityController))->Kp = Kp;
		((PID_t*)(Car.RightFrontMotor->VelocityController))->Ki = Ki;
		((PID_t*)(Car.RightFrontMotor->VelocityController))->Kd = Kd;
		COS_Logf(info,"%s has set pid Kp=%.2f Ki=%.2f Kd=%.2f.",Car.RightFrontMotor->Name,
				((PID_t*)(Car.RightFrontMotor->VelocityController))->Kp,
				((PID_t*)(Car.RightFrontMotor->VelocityController))->Ki,
				((PID_t*)(Car.RightFrontMotor->VelocityController))->Kd);
		break;
	}
	case 3:
	{
		((PID_t*)(Car.RightRearMotor->VelocityController))->Kp = Kp;
		((PID_t*)(Car.RightRearMotor->VelocityController))->Ki = Ki;
		((PID_t*)(Car.RightRearMotor->VelocityController))->Kd = Kd;
		COS_Logf(info,"%s has set pid Kp=%.2f Ki=%.2f Kd=%.2f.",Car.RightRearMotor->Name,
				((PID_t*)(Car.RightRearMotor->VelocityController))->Kp,
				((PID_t*)(Car.RightRearMotor->VelocityController))->Ki,
				((PID_t*)(Car.RightRearMotor->VelocityController))->Kd);
		break;
	}
	default:
		break;
	}
}




/*****************************************************************************************************
 * @name:SetCarVelocitycb
 * @brief:callback funtion of command 'scv'
 * @params:
 *     1.ParamList:The parameters' data passed from command processor.
 *     1.1.Angle:Angle of velocity.
 *     1.2.Velocity:value of velocity.
 *     1.3.AngularVel:Angular velocity of rotation.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void SetCarVelocitycb(uint8_t* ParamList)
{
	uint8_t  SizeSum    = 0;
	float    Angle      = 0;
	float    Velocity   = 0;
	float    AngularVel = 0;

	Angle      = *(uint16_t*)(ParamList + SizeSum);
	SizeSum   += sizeof(uint16_t);
	Velocity   = *(float*)   (ParamList + SizeSum);
	SizeSum   += sizeof(float);
	AngularVel = *(float*)   (ParamList + SizeSum);

	Angle = Angle * 3.141593 / 180.0;
	Car_SetVelocity(&Car, Angle, Velocity, AngularVel);
}






/*****************************************************************************************************
 * @name:MotorPIDSelfAdjustcb
 * @brief:
 * @params:
 *     1.ParamList:The parameters' data passed from command processor.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void MotorPIDSelfAdjustcb(uint8_t* ParamList)
{
	/* to be developed... */
	return ;
}










#endif
#ifdef _cplusplus
}
#endif
