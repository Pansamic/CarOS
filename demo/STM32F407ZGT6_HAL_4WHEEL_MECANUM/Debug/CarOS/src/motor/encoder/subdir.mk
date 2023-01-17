################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CarOS/src/motor/encoder/c_encoder.c 

OBJS += \
./CarOS/src/motor/encoder/c_encoder.o 

C_DEPS += \
./CarOS/src/motor/encoder/c_encoder.d 


# Each subdirectory must supply rules for building sources it contributes
CarOS/src/motor/encoder/%.o CarOS/src/motor/encoder/%.su: ../CarOS/src/motor/encoder/%.c CarOS/src/motor/encoder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__TARGET_FPU_VFP -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../CarOS -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CarOS-2f-src-2f-motor-2f-encoder

clean-CarOS-2f-src-2f-motor-2f-encoder:
	-$(RM) ./CarOS/src/motor/encoder/c_encoder.d ./CarOS/src/motor/encoder/c_encoder.o ./CarOS/src/motor/encoder/c_encoder.su

.PHONY: clean-CarOS-2f-src-2f-motor-2f-encoder

