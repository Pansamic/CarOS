/****************************************************************************
Copyright (C), 2022, pansamic(王耕杰 Wang GengJie) pansamic@foxmail.com
FileName: DCmotor_driver.h
Author:   pansamic  
Version:  v0.0
Date:     2022.12.17
Description:

History:
<author>     <time>     <version>    <desc>
pansamic   2022.12.17      v0.0
****************************************************************************/
#ifndef __DCMOTOR_DRIVER__
#define __DCMOTOR_DRIVER__
#ifdef __cplusplus
extern "C" {
#endif
#include "./src/c_includes.h"
#include "./src/motor/DCMotor/driver/L298N/L298N_Driver.h"

/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/
typedef struct DCMotorDefinition DCMotor;

/*****************************************************************************************************
 *                                                                                                   *
 *                                         FUNCTION                                                  *
 *                                                                                                   *
 *****************************************************************************************************/
#if USE_HAL_LIB
void DCMotor_AddL298N( DCMotor *Motor, GPIO_TypeDef *Port1, uint16_t Pin1, GPIO_TypeDef *Port2, uint16_t Pin2, TIM_HandleTypeDef *htim, uint32_t Channel );
#elif USE_FW_LIB

#elif USE_MSP432

#endif
#ifdef __cplusplus
}
#endif
#endif
