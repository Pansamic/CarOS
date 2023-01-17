/*****************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    c_types.h
Author:      Pansamic
Version:     1.0
Create date: 2023.1.3
Description:
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2023.1.3    create v1.0 version.
*****************************************************************************/
#ifndef __C_TYPES_H__
#define __C_TYPES_H__
#ifdef __cplusplus
extern "C" {
#endif

#define LinVelToAngVel(Radius,LinearVel)  LinearVel/Radius
#define AngVelToLinVel(Radius,AngularVel) Radius*AngularVel

/* it is used to describe rotation speed of dc motor, unit:r/s */
typedef float Angle_t;       // unit:rad
typedef float LinVelocity_t; // unit:cm/s
typedef float AngVelocity_t; // unit:rad/s


/* Remember that COS_uart is ALWAYS a pointer.
 * In HAL lib, it's like '&huart1', etc.
 * In FW lib, it's like 'USART1', etc.,because 'USART1' itself is a pointer.
 * In MSP432, it's like 'EUSCI_A0_BASE', etc., because 'EUSCI_A0_BASE' itself is a pointer. */
#if USE_HAL_LIB
#define COS_uart UART_HandleTypeDef*
#elif USE_FW_LIB
#define COS_uart USART_TypeDef*
#elif USE_MSP432
#define COS_uart void*
#endif

#ifdef __cplusplus
}
#endif
#endif
