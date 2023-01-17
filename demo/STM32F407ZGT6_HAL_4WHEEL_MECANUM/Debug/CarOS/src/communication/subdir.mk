################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CarOS/src/communication/c_command.c \
../CarOS/src/communication/c_io.c \
../CarOS/src/communication/c_pkgproc.c 

OBJS += \
./CarOS/src/communication/c_command.o \
./CarOS/src/communication/c_io.o \
./CarOS/src/communication/c_pkgproc.o 

C_DEPS += \
./CarOS/src/communication/c_command.d \
./CarOS/src/communication/c_io.d \
./CarOS/src/communication/c_pkgproc.d 


# Each subdirectory must supply rules for building sources it contributes
CarOS/src/communication/%.o CarOS/src/communication/%.su: ../CarOS/src/communication/%.c CarOS/src/communication/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__TARGET_FPU_VFP -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../CarOS -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CarOS-2f-src-2f-communication

clean-CarOS-2f-src-2f-communication:
	-$(RM) ./CarOS/src/communication/c_command.d ./CarOS/src/communication/c_command.o ./CarOS/src/communication/c_command.su ./CarOS/src/communication/c_io.d ./CarOS/src/communication/c_io.o ./CarOS/src/communication/c_io.su ./CarOS/src/communication/c_pkgproc.d ./CarOS/src/communication/c_pkgproc.o ./CarOS/src/communication/c_pkgproc.su

.PHONY: clean-CarOS-2f-src-2f-communication

