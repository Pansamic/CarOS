################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CarOS/src/app/c_app.c \
../CarOS/src/app/c_builtinapp.c 

OBJS += \
./CarOS/src/app/c_app.o \
./CarOS/src/app/c_builtinapp.o 

C_DEPS += \
./CarOS/src/app/c_app.d \
./CarOS/src/app/c_builtinapp.d 


# Each subdirectory must supply rules for building sources it contributes
CarOS/src/app/%.o CarOS/src/app/%.su: ../CarOS/src/app/%.c CarOS/src/app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__TARGET_FPU_VFP -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../CarOS -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CarOS-2f-src-2f-app

clean-CarOS-2f-src-2f-app:
	-$(RM) ./CarOS/src/app/c_app.d ./CarOS/src/app/c_app.o ./CarOS/src/app/c_app.su ./CarOS/src/app/c_builtinapp.d ./CarOS/src/app/c_builtinapp.o ./CarOS/src/app/c_builtinapp.su

.PHONY: clean-CarOS-2f-src-2f-app

