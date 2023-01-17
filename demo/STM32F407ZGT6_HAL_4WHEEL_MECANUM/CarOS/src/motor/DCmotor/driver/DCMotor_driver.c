/****************************************************************************
Copyright (C), 2022, pansamic(王耕杰 Wang GengJie) pansamic@foxmail.com
FileName: DCmotor_driver.c
Author:   pansamic
Version:  v0.0
Date:     2023.1.3
Description:

History:
<author>     <time>     <version>    <desc>
pansamic   2023.1.3      v0.0
****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#include "./src/memory/c_memory.h"
#include "./src/debug/c_debug.h"
#include "./src/motor/DCmotor/driver/DCMotor_Driver.h"

/*****************************************************************************************************
 * @name:DCMotor_AddL298N
 * @brief:add a L298N driver model to dc motor model and initialize the L298N driver.
 * @params:
 *     1.Motor:pointer of struct 'DCMotor'.
 *     2.Port1:logical controlling GPIO pin1 port.
 *     3.Pin1:logical controlling GPIO pin1.
 *     4.Port2:logical controlling GPIO pin2 port.
 *     5.Pin2:logical controlling GPIO pin2.
 *     6.htim:pointer of timer instance that generates PWM wave.
 *     7.Channel:the channel of timer that this motor uses.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
#if USE_HAL_LIB
void DCMotor_AddL298N(DCMotor *Motor, GPIO_TypeDef *Port1, uint16_t Pin1, GPIO_TypeDef *Port2, uint16_t Pin2, TIM_HandleTypeDef *htim, uint32_t Channel)
{
	L298N *NewL298N = (L298N*)cos_malloc(sizeof(L298N));
	if(NewL298N) // allocate memory successfully
	{
		L298N_Init(NewL298N, Port1, Pin1, Port2, Pin2, htim, Channel);

		/* mount L298N instance on motor */
		Motor->Driver = NewL298N;

		/* set velocity application function as L298N's */
		Motor->DriverSetVelocitycb = L298N_SetMotorVelocity;

		/* if the motor doesn't have name, that means the motor has not been initialized yet */
		if(Motor->Name!=NULL)
		{
			COS_Logf(info, "%s has mounted L298N driver.", Motor->Name);
		}
		else
		{
			COS_Logf(warning, "L298N driver is mounted, but motor hasn't been initialized yet. Make sure initialize motor before mount driver.");
		}
	}
	else
	{
		COS_Logf(warning,"%s mount L298N driver fail:stack have no space, check memory module or increase memory size.", Motor->Name);
	}
}
#elif USE_FW_LIB

#elif USE_MSP432

#endif
#ifdef __cplusplus
}
#endif
