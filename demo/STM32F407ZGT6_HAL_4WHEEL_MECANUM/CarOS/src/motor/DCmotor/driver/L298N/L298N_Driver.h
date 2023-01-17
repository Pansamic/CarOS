/****************************************************************************
Copyright (C), 2022, pansamic(王耕杰 Wang GengJie) pansamic@foxmail.com
FileName: L298N-Driver.h
Author:   pansamic  
Version:  v1.0
Date:     2022.12.9
Description:L298N module driver, not L298N chip. This module satisfies general
    interface of DCMotor model.
History:
<author>     <time>     <version>    <desc>
pansamic   2022.12.9      v0.0
****************************************************************************/
#ifndef __L298N_DRIVER__
#define __L298N_DRIVER__
#ifdef __cplusplus
extern "C" {
#endif

#include "./src/c_includes.h"
#include "./src/motor/DCmotor/c_DCMotor.h"


/*****************************************************************************************
 *                                                                                       *
 *                                      MACROS                                           *
 *                                                                                       *
 *****************************************************************************************/


/*****************************************************************************************
 *                                                                                       *
 *                                      TYPEDEF                                          *
 *                                                                                       *
 *****************************************************************************************/
/*****************************************************************************************
 * @brief:L298N driver model, containing some necessary attributes of L298N driver.
 * @param:
 *     1.RotationControlGPIOPin1:L298N module needs 2 GPIOs to control motor rotation, so
 *         the two GPIOs is supposed to be set as 'output' mode.
 *     2.RotationControlGPIOPort1:See above.
 *     3.RotationControlGPIOPin2:See above.
 *     4.RotationControlGPIOPort2:See above.
 *     5.PWMGeneratingTimer:PWM wave generator timer instance, different with different MCU
 *         and different platform.
 *     6.PWMGeneratingTimerChannel:literal meaning.
 *     7.PWMCounterPeriod:Usually the maximum count value.
 * */
typedef struct L298N
{
#if USE_STM32
#if USE_HAL_LIB
	uint16_t              RotationControlGPIOPin1;   // e.g. GPIO_PIN_1
	GPIO_TypeDef         *RotationControlGPIOPort1;  // e.g. GPIOC
	uint16_t              RotationControlGPIOPin2;
	GPIO_TypeDef         *RotationControlGPIOPort2;
	TIM_HandleTypeDef    *PWMGeneratingTimer;        // e.g. htim1
	uint32_t              PWMGeneratingTimerChannel; // e.g. TIM_CHANNEL_1

#elif USE_FW_LIB

#endif
#elif USE_MSP432

#endif


}L298N;

/*****************************************************************************************
 *                                                                                       *
 *                                     FUNCTION                                          *
 *                                                                                       *
 *****************************************************************************************/
#if USE_HAL_LIB
void   L298N_Init             ( void* L298N_instance, GPIO_TypeDef *Port1, uint16_t Pin1, GPIO_TypeDef *Port2, uint16_t Pin2, TIM_HandleTypeDef *htim, uint32_t Channel );
#elif USE_FW_LIB

#elif USE_MSP432

#endif
void   L298N_SetMotorVelocity ( void* L298N_instance, int32_t TimerCounterLoadVal );

#ifdef __cplusplus
}
#endif
#endif
