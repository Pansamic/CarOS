################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.c 

OBJS += \
./CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.o 

C_DEPS += \
./CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.d 


# Each subdirectory must supply rules for building sources it contributes
CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/%.o CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/%.su: ../CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/%.c CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__TARGET_FPU_VFP -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../CarOS -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CarOS-2f-src-2f-peri_driver-2f-distance_measure_sensor-2f-laser_TF_Luna

clean-CarOS-2f-src-2f-peri_driver-2f-distance_measure_sensor-2f-laser_TF_Luna:
	-$(RM) ./CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.d ./CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.o ./CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/c_tf_luna.su

.PHONY: clean-CarOS-2f-src-2f-peri_driver-2f-distance_measure_sensor-2f-laser_TF_Luna

