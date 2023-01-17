/****************************************************************************
Copyright (C), 2022, pansamic(王耕杰 Wang GengJie) pansamic@foxmail.com
FileName: L298N-Driver.c
Author:   pansamic  
Version:  v1.0
Date:     2022.12.9
Description:

History:
<author>     <time>     <version>    <desc>
pansamic   2022.12.9      v0.0
****************************************************************************/
#include <stdlib.h>
#include "./src/debug/c_debug.h"
#include "./src/motor/DCmotor/driver/L298N/L298N_Driver.h"
#include "./src/memory/c_memory.h"


static void L298N_SetRotateForward   (L298N *L298N_instance);
static void L298N_SetRotateBackward  (L298N *L298N_instance);
static void L298N_StopMotor          (L298N *L298N_instance);




/*****************************************************************************************************
 * @name:L298N_Init
 * @brief:the function configures MCU timer related hardware to enables PWM generation.
 * @params:
 *     1.L298N_instance:pointer of L298N struct. In order to fit general format of DCMotor driver,
 *         init function must has void* pointer to fit all types of struct of driver model.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
#if USE_HAL_LIB
void L298N_Init(void* L298N_instance, GPIO_TypeDef *Port1, uint16_t Pin1, GPIO_TypeDef *Port2, uint16_t Pin2, TIM_HandleTypeDef *htim, uint32_t Channel)
{
	L298N* pL298N = (L298N*)L298N_instance;
	pL298N->RotationControlGPIOPin1 = Pin1;
	pL298N->RotationControlGPIOPort1 = Port1;
	pL298N->RotationControlGPIOPin2 = Pin2;
	pL298N->RotationControlGPIOPort2 = Port2;
	pL298N->PWMGeneratingTimer = htim;
	pL298N->PWMGeneratingTimerChannel = Channel;
	/* firstly start pwm generation, otherwise timer won't generate pwm wave */
	HAL_TIM_PWM_Start(pL298N->PWMGeneratingTimer,pL298N->PWMGeneratingTimerChannel);
}
#elif USE_FW_LIB

#elif USE_MSP432

#endif


/*****************************************************************************************************
 * @name:L298N_SetMotorVelocity
 * @brief:this function is the function most close to hardware because this function calls timer
 *     capture/compare counter register.
 * @params:
 *     1.L298N_instance:pointer of L298N struct. In order to fit general format of DCMotor driver,
 *         setspeed function must has void* pointer to fit all types of struct of driver model.
 *     2.TimerCounterLoadVal:set timer capture/compare register.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void L298N_SetMotorVelocity(void* L298N_instance, int32_t TimerCounterLoadVal)
{
	L298N* pL298N = (L298N*)L298N_instance;
#if USE_STM32
#if USE_HAL_LIB
	if(TimerCounterLoadVal==0)
	{
		__HAL_TIM_SET_COMPARE(pL298N->PWMGeneratingTimer, pL298N->PWMGeneratingTimerChannel, 2);
		L298N_StopMotor(pL298N);
	}
	else if(TimerCounterLoadVal>0)
	{
		L298N_SetRotateForward(pL298N);
		__HAL_TIM_SET_COMPARE(pL298N->PWMGeneratingTimer, pL298N->PWMGeneratingTimerChannel, (uint32_t)TimerCounterLoadVal);
	}
	else
	{
		L298N_SetRotateBackward(pL298N);
		__HAL_TIM_SET_COMPARE(pL298N->PWMGeneratingTimer, pL298N->PWMGeneratingTimerChannel, (uint32_t)abs(TimerCounterLoadVal));
	}
#elif USE_FW_LIB

#endif
#elif USE_MSP432

#endif
}



/*****************************************************************************************************
 * @name:L298N_SetRotateForward
 * @brief:L298N module has two logic pins which controls rotation direction of motor, so this function
 *     changes pin state to set motor rotate forward.
 * @params:
 *     1.L298N_instance:pointer of struct L298N.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void L298N_SetRotateForward(L298N* L298N_instance)
{
#if USE_STM32
#if USE_HAL_LIB

		HAL_GPIO_WritePin(L298N_instance->RotationControlGPIOPort1,L298N_instance->RotationControlGPIOPin1,GPIO_PIN_SET);
		HAL_GPIO_WritePin(L298N_instance->RotationControlGPIOPort2,L298N_instance->RotationControlGPIOPin2,GPIO_PIN_RESET);

#elif USE_FW_LIB

		GPIO_Setbits(L298N_instance->RotationControlGPIOPort1,L298N_instance->RotationControlGPIOPin1);
		GPIO_Resetbits(L298N_instance->RotationControlGPIOPort2,L298N_instance->RotationControlGPIOPin2);

#endif
#elif USE_MSP432

#endif

}



/*****************************************************************************************************
 * @name:L298N_SetRotateBackward
 * @brief:L298N module has two logic pins which controls rotation direction of motor, so this function
 *     changes pin state to set motor rotate backward.
 * @params:
 *     1.L298N_instance:pointer of struct L298N.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void L298N_SetRotateBackward(L298N* L298N_instance)
{
#if USE_STM32
#if USE_HAL_LIB
	HAL_GPIO_WritePin(L298N_instance->RotationControlGPIOPort1,L298N_instance->RotationControlGPIOPin1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(L298N_instance->RotationControlGPIOPort2,L298N_instance->RotationControlGPIOPin2,GPIO_PIN_SET);
#elif USE_FW_LIB
	GPIO_Resetbits(L298N_instance->RotationControlGPIOPort1,L298N_instance->RotationControlGPIOPin1);
	GPIO_Setbits(L298N_instance->RotationControlGPIOPort2,L298N_instance->RotationControlGPIOPin2);
#endif
#elif USE_MSP432

#endif

}

/*****************************************************************************************************
 * @name:L298N_StopMotor
 * @brief:stop motor
 * @params:
 *     1.L298N_instance:pointer of L298N instance.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
static void L298N_StopMotor(L298N *L298N_instance)
{
#if USE_STM32
#if USE_HAL_LIB

	HAL_GPIO_WritePin(L298N_instance->RotationControlGPIOPort1,L298N_instance->RotationControlGPIOPin1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(L298N_instance->RotationControlGPIOPort2,L298N_instance->RotationControlGPIOPin2,GPIO_PIN_RESET);

#elif USE_FW_LIB

	GPIO_Resetbits(L298N_instance->RotationControlGPIOPort1,L298N_instance->RotationControlGPIOPin1);
	GPIO_Resetbits(L298N_instance->RotationControlGPIOPort2,L298N_instance->RotationControlGPIOPin2);

#endif
#elif USE_MSP432

#endif
}

