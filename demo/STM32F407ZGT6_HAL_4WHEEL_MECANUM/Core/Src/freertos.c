/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "tim.h"
#include "cos.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern DCMotor LeftFrontMotor;
extern DCMotor LeftRearMotor;
extern DCMotor RightFrontMotor;
extern DCMotor RightRearMotor;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId Task_AdjustSpeedHandle;
osThreadId Task_CarOSioProcessHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void AdjustSpeed(void const * argument);
void CarOS_ioProcess(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Task_AdjustSpeed */
  osThreadDef(Task_AdjustSpeed, AdjustSpeed, osPriorityAboveNormal, 0, 512);
  Task_AdjustSpeedHandle = osThreadCreate(osThread(Task_AdjustSpeed), NULL);

  /* definition and creation of Task_CarOSioProcess */
  osThreadDef(Task_CarOSioProcess, CarOS_ioProcess, osPriorityIdle, 0, 512);
  Task_CarOSioProcessHandle = osThreadCreate(osThread(Task_CarOSioProcess), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_AdjustSpeed */
/**
* @brief Function implementing the Task_AdjustSpeed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_AdjustSpeed */
void AdjustSpeed(void const * argument)
{
  /* USER CODE BEGIN AdjustSpeed */
	TickType_t xLastWakeTime;
	/* delay time must equal to Motor.Encoder.Interval */
	const TickType_t xFrequency = 10;

	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  while(1)
  {
	  vTaskDelayUntil( &xLastWakeTime, xFrequency );

	  Car_AdjustedVelocity(&Car);

//	  COS_printf("%.2f %.2f %.2f %.2f\r\n",LeftFrontMotor.CurrentVelocity,
//			                           LeftRearMotor.CurrentVelocity,
//							           RightFrontMotor.CurrentVelocity,
//							           RightRearMotor.CurrentVelocity);

  }
  /* USER CODE END AdjustSpeed */
}

/* USER CODE BEGIN Header_CarOS_ioProcess */
/**
* @brief Function implementing the Task_CarOSioProcess thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CarOS_ioProcess */
void CarOS_ioProcess(void const * argument)
{
  /* USER CODE BEGIN CarOS_ioProcess */
  /* Infinite loop */
  for(;;)
  {
    osDelay(50);
    io_Process();
  }
  /* USER CODE END CarOS_ioProcess */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
