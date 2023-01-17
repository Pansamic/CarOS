/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOF
#define LFMotor_RotateDirectionCtrl1_Pin GPIO_PIN_0
#define LFMotor_RotateDirectionCtrl1_GPIO_Port GPIOC
#define LFMotor_RotateDirectionCtrl2_Pin GPIO_PIN_1
#define LFMotor_RotateDirectionCtrl2_GPIO_Port GPIOC
#define LRMotor_RotateDirectionCtrl1_Pin GPIO_PIN_2
#define LRMotor_RotateDirectionCtrl1_GPIO_Port GPIOC
#define LRMotor_RotateDirectionCtrl2_Pin GPIO_PIN_3
#define LRMotor_RotateDirectionCtrl2_GPIO_Port GPIOC
#define EncA_RightRear_Pin GPIO_PIN_0
#define EncA_RightRear_GPIO_Port GPIOA
#define EncB_RightRear_Pin GPIO_PIN_1
#define EncB_RightRear_GPIO_Port GPIOA
#define EncA_LeftFront_Pin GPIO_PIN_5
#define EncA_LeftFront_GPIO_Port GPIOA
#define EncA_LeftRear_Pin GPIO_PIN_6
#define EncA_LeftRear_GPIO_Port GPIOA
#define EncB_LeftRear_Pin GPIO_PIN_7
#define EncB_LeftRear_GPIO_Port GPIOA
#define RFMotor_RotateDirectionCtrl1_Pin GPIO_PIN_4
#define RFMotor_RotateDirectionCtrl1_GPIO_Port GPIOC
#define RFMotor_RotateDirectionCtrl2_Pin GPIO_PIN_5
#define RFMotor_RotateDirectionCtrl2_GPIO_Port GPIOC
#define PWM_LeftFrontMotor_Pin GPIO_PIN_9
#define PWM_LeftFrontMotor_GPIO_Port GPIOE
#define PWM_LeftRearMotor_Pin GPIO_PIN_11
#define PWM_LeftRearMotor_GPIO_Port GPIOE
#define PWM_RightFrontMotor_Pin GPIO_PIN_13
#define PWM_RightFrontMotor_GPIO_Port GPIOE
#define PWM_RightRearMotor_Pin GPIO_PIN_14
#define PWM_RightRearMotor_GPIO_Port GPIOE
#define EncB_RightFront_Pin GPIO_PIN_12
#define EncB_RightFront_GPIO_Port GPIOD
#define EncA_RightFront_Pin GPIO_PIN_13
#define EncA_RightFront_GPIO_Port GPIOD
#define RRMotor_RotateDirectionCtrl1_Pin GPIO_PIN_6
#define RRMotor_RotateDirectionCtrl1_GPIO_Port GPIOC
#define RRMotor_RotateDirectionCtrl2_Pin GPIO_PIN_7
#define RRMotor_RotateDirectionCtrl2_GPIO_Port GPIOC
#define ConsoleTx_Pin GPIO_PIN_9
#define ConsoleTx_GPIO_Port GPIOA
#define ConsoleRx_Pin GPIO_PIN_10
#define ConsoleRx_GPIO_Port GPIOA
#define EncB_LeftFront_Pin GPIO_PIN_3
#define EncB_LeftFront_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
