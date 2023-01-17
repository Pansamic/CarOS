#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "./c_config.h"
#include "./src/c_types.h"
#if USE_STM32
#if USE_STM32F1
#if USE_HAL_LIB
#include "stm32f1xx_hal.h"
#elif USE_FW_LIB

#endif/* USE_HAL_LIB */
#endif/* USE_STM32F1 */

#if USE_STM32F4
#if USE_HAL_LIB
#include "stm32f4xx_hal.h"
#elif USE_FW_LIB

#endif/* USE_HAL_LIB */
#endif/* USE_STM32F4 */

#endif/* USE_STM32 */
#if USE_MSP432

#endif

#ifdef __cplusplus
}
#endif
#endif /* __INCLUDES_H__ */
