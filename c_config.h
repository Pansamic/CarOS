/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_config.h
Author:      Pansamic
Version:     1.1
Create date: 2022.12.09
Description: the file contains some config macros that controls almost all of
    features of a car. This file is the first file user must modify.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2022.12.10         create v1.0 version.
*****************************************************************************/
#ifndef __C_CONFIG_H_
#define __C_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION     1.3

/*****************************************************************************************************
 *                                                                                                   *
 *                                           MCU                                                     *
 *                                                                                                   *
 *****************************************************************************************************/
#define USE_STM32                  1
#define USE_MSP432                 0


/*****************************************************************************************************
 *                                                                                                   *
 *                                    STM32 configurations                                           *
 *                                                                                                   *
 *****************************************************************************************************/
#if USE_STM32
	#define USE_HAL_LIB            1      // use hal library, usually developed with STM32CubeIDE or STM32CubeMX
	#define USE_FW_LIB             0      // use firmware library, usually developed with keil5
	#define USE_STM32F4            1      // use STM32F4 series, with FPU
	#define USE_STM32F1            0      // use STM32F1 series, without FPU
    #define USE_STM32G4            0      // use STM32G4 series, with FPU
#endif

/*****************************************************************************************************
 *                                                                                                   *
 *                                    Hardware Configuration                                         *
 *                                                                                                   *
 *****************************************************************************************************/
#define MEM_BLOCK_SIZE    16          // memory block size, suggested value:8, unit:byte
#define MEM_MAX_SIZE      10           // maximum RAM size for CarOS, can't be lower than 2. unit:kb
#define MEM_ALLOC_TABLE_SIZE MEM_MAX_SIZE*1024/MEM_BLOCK_SIZE  // memory table size

/*****************************************************************************************************
 *                                                                                                   *
 *                                      Car Configuration                                            *
 *                                                                                                   *
 *****************************************************************************************************/
#define FOUR_WHEEL_MECANUM      1
#define SIX_WHEEL_MECANUM       2
#define TWO_WHEEL_BALANCE       3
#define AKERMAN                 4
#define FOUR_WHEEL_DIFFERENTIAL 5
#define TWO_WHEEL_DIFFERENTIAL  6
#define THREE_WHEEL_OMNI        7
#define FOUR_WHEEL_OMNI         8
#define CAR_TYPE FOUR_WHEEL_MECANUM  // Choose the model of the car

/*****************************************************************************************************
 *                                                                                                   *
 *                                   Encoder Configuration                                           *
 *                                                                                                   *
 *****************************************************************************************************/

/*****************************************************************************************************
 *                                                                                                   *
 *                                 Debug System Configuration                                        *
 *                                                                                                   *
 *****************************************************************************************************/
/* COS system debug Enable */
#define COS_DEBUG 1

/* COS components debug Enable */
#if COS_DEBUG==1

#define DEBUG_CAR     1
#define DEBUG_DCMOTOR 1
#define DEBUG_ENCODER 1

/* Motor debug configuration */
#if DEBUG_DCMOTOR==1
	#define DEBUG_L298N   1
	#define DEBUG_A4950   1
#endif

/* if this macro equals to 1, CarOS system will print
 * log information realtime */
#define LOG_PRINT         1

/* if this macro equals 1, CarOS system will store
 * log information in RAM or other storage meterials */
#define LOG_STORAGE       1

/* Log level enable */
#define LOG_EMERG         1
#define LOG_ALERT         1
#define LOG_CRIT          1
#define LOG_ERR           1
#define LOG_WARNING       1
#define LOG_NOTICE        1
#define LOG_INFO          1
#define LOG_DEBUG         0   // debug mode, records almost everything, but occupies much memory and MCU processing speed

#if LOG_STORAGE
#define LOG_MEM_SIZE      10  // no lower than 2KB is recommended. unit:kb
#endif

#endif/* COS_DEBUG==1 */

/*****************************************************************************************************
 *                                                                                                   *
 *                                 Communication Configuration                                       *
 *                                                                                                   *
 *****************************************************************************************************/
#define INPUT_BUFFER_SIZE   1     // system input buffer, unit:kb
#define OUTPUT_BUFFER1_SIZE 10    // system main output buffer, unit:kb
#define OUTPUT_BUFFER2_SIZE 5     // system secondary output buffer, can be smaller than main buffer. unit:kb
#define USE_USART_DMA       0     // use DMA to transmit usart data, which is much faster than block way.
                                  // but user must configure DMA first. CarOS won't configure DMA automatically.




















#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif /*__C_CONFIG_H_*/
