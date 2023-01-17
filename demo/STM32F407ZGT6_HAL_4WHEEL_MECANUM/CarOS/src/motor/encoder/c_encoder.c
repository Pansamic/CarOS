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
#ifdef __cplusplus
extern "C" {
#endif
#include "./src/motor/encoder/c_encoder.h"


/*****************************************************************************************************
 * @name:EncoderInit
 * @brief:Initialize encoder.
 * @params:
 *     1.Enc:Pointer of Encoder.
 *     2.htim:pointer of timer, whose type varies from platform to platform.
 *     3.PulsePerRound:literal meaning.
 *     4.Interval:update period of encoder.
 * @retval:none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
#if USE_HAL_LIB
void EncoderInit(Encoder *Enc, TIM_HandleTypeDef *htim, uint16_t PulsePerRound, uint16_t Interval)
{
	Enc->PulsePerRound = PulsePerRound;
	Enc->Interval = Interval;
	Enc->htim = htim;
	Enc->CurrentCount = 0;
	HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_1|TIM_CHANNEL_2);
}
#elif USE_FW_LIB
void EncoderInit(Encoder *Enc, TIM_TypeDef *htim, uint16_t PulsePerRound, uint16_t Interval)
{
	Enc->PulsePerRound = PulsePerRound;
	Enc->Interval = Interval;
	Enc->htim = htim;
	TIM_EncoderInterfaceConfig(htim, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(htim, ENABLE);
}

#elif USE_MSP432

#endif

/*****************************************************************************************************
 * @name:EncoderUpdate
 * @brief:get encoder count value from MCU timer and update encoder member 'CurrentCount'.
 * @params:
 *     1.Enc:pointer of Encoder instance.
 * @retval: none
 * @author: Wang Geng Jie
 *****************************************************************************************************/
void EncoderUpdate(Encoder *Enc)
{
#if USE_HAL_LIB
	/* since STM32 timer max count value is 65535 and the count value will decrease from 65535 if
	 * the wheel rotate counterclockwise and will increase from 0 if wheel rotates clockwise,
	 * type cast the count value to signed short will get negative number when wheel rotates
	 * counterclockwise. Therefore we get negative number and positive number which represent
	 * rotate forward and backward respectively. */
    Enc->CurrentCount = (short)__HAL_TIM_GET_COUNTER(Enc->htim);

    /* clear the count value, otherwise the count number will add up continuously */
	__HAL_TIM_GET_COUNTER(Enc->htim) = 0;
#elif USE_FW_LIB

#elif USE_MSP432

#endif
}

#ifdef __cplusplus
}
#endif
