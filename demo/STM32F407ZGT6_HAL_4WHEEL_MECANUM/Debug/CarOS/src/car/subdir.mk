################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CarOS/src/car/c_2wheel_balance.c \
../CarOS/src/car/c_2wheel_differential.c \
../CarOS/src/car/c_3wheel_omni.c \
../CarOS/src/car/c_4wheel_differential.c \
../CarOS/src/car/c_4wheel_mecanum.c \
../CarOS/src/car/c_4wheel_omni.c \
../CarOS/src/car/c_6wheel_mecanum.c \
../CarOS/src/car/c_akerman.c \
../CarOS/src/car/c_car.c 

OBJS += \
./CarOS/src/car/c_2wheel_balance.o \
./CarOS/src/car/c_2wheel_differential.o \
./CarOS/src/car/c_3wheel_omni.o \
./CarOS/src/car/c_4wheel_differential.o \
./CarOS/src/car/c_4wheel_mecanum.o \
./CarOS/src/car/c_4wheel_omni.o \
./CarOS/src/car/c_6wheel_mecanum.o \
./CarOS/src/car/c_akerman.o \
./CarOS/src/car/c_car.o 

C_DEPS += \
./CarOS/src/car/c_2wheel_balance.d \
./CarOS/src/car/c_2wheel_differential.d \
./CarOS/src/car/c_3wheel_omni.d \
./CarOS/src/car/c_4wheel_differential.d \
./CarOS/src/car/c_4wheel_mecanum.d \
./CarOS/src/car/c_4wheel_omni.d \
./CarOS/src/car/c_6wheel_mecanum.d \
./CarOS/src/car/c_akerman.d \
./CarOS/src/car/c_car.d 


# Each subdirectory must supply rules for building sources it contributes
CarOS/src/car/%.o CarOS/src/car/%.su: ../CarOS/src/car/%.c CarOS/src/car/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DARM_MATH_CM4 -D__FPU_PRESENT=1 -D__TARGET_FPU_VFP -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../CarOS -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CarOS-2f-src-2f-car

clean-CarOS-2f-src-2f-car:
	-$(RM) ./CarOS/src/car/c_2wheel_balance.d ./CarOS/src/car/c_2wheel_balance.o ./CarOS/src/car/c_2wheel_balance.su ./CarOS/src/car/c_2wheel_differential.d ./CarOS/src/car/c_2wheel_differential.o ./CarOS/src/car/c_2wheel_differential.su ./CarOS/src/car/c_3wheel_omni.d ./CarOS/src/car/c_3wheel_omni.o ./CarOS/src/car/c_3wheel_omni.su ./CarOS/src/car/c_4wheel_differential.d ./CarOS/src/car/c_4wheel_differential.o ./CarOS/src/car/c_4wheel_differential.su ./CarOS/src/car/c_4wheel_mecanum.d ./CarOS/src/car/c_4wheel_mecanum.o ./CarOS/src/car/c_4wheel_mecanum.su ./CarOS/src/car/c_4wheel_omni.d ./CarOS/src/car/c_4wheel_omni.o ./CarOS/src/car/c_4wheel_omni.su ./CarOS/src/car/c_6wheel_mecanum.d ./CarOS/src/car/c_6wheel_mecanum.o ./CarOS/src/car/c_6wheel_mecanum.su ./CarOS/src/car/c_akerman.d ./CarOS/src/car/c_akerman.o ./CarOS/src/car/c_akerman.su ./CarOS/src/car/c_car.d ./CarOS/src/car/c_car.o ./CarOS/src/car/c_car.su

.PHONY: clean-CarOS-2f-src-2f-car

