/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_encoder.h
Author:      Pansamic
Version:     1.0
Create date: 2022.12.19
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2022.12.19    create v1.0 version.
*****************************************************************************/
#ifndef __C_ENCODER__
#define __C_ENCODER__
#ifdef __cplusplus
extern "C" {
#endif
#include "./src/c_includes.h"
/**********************************************************************************************
 *                                                                                            *
 *                                         MACROS                                             *
 *                                                                                            *
 **********************************************************************************************/

/**********************************************************************************************
 *                                                                                            *
 *                                         TYPEDEF                                            *
 *                                                                                            *
 **********************************************************************************************/
typedef struct EncoderDefinition
{
#if USE_HAL_LIB
	TIM_HandleTypeDef *htim;
#elif USE_FW_LIB

#elif USE_MSP432

#endif
	short              CurrentCount;
	uint16_t           PulsePerRound;
	uint16_t           Interval;  // unit:ms

}Encoder;

/**********************************************************************************************
 *                                                                                            *
 *                                     GLOBAL FUNCTION                                        *
 *                                                                                            *
 **********************************************************************************************/
#if USE_HAL_LIB
void EncoderInit   (Encoder *Enc, TIM_HandleTypeDef *htim, uint16_t PulsePerRound, uint16_t Interval);
#elif USE_FW_LIB

#elif USE_MSP432

#endif

void EncoderUpdate (Encoder *Enc);

#ifdef __cplusplus
}
#endif
#endif
