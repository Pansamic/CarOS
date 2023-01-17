/*****************************************************************************************************
THIS PROGRAM IS FREE SOFTWARE. YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT
UNDER THE TERMS OF THE GNU GPLV3 AS PUBLISHED BY THE FREE SOFTWARE FOUNDATION.

Copyright (C), 2022-2023, pansamic(Wang GengJie) pansamic@foxmail.com

Filename:    cos.h
Author:      Pansamic
Version:     1.1
Create date: 2022.12.09
Description: the header of all features of CarOS.
Others:      none

History:
1. <author>    <date>                  <desc>
   pansamic  2022.12.10         create v1.0 version.
   pansamic  2023.1.3           v1.1 done
*****************************************************************************/
#ifndef __COS_H_
#define __COS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "./src/car/c_car.h"
#include "./src/memory/c_memory.h"
#include "./src/communication/c_io.h"
#include "./src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.h"
#include "./src/peri_driver/peri_driver.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif
