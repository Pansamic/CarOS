################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include CarOS/src/peri_driver/distance_measure_sensor/laser_TF_Luna/subdir.mk
-include CarOS/src/motor/encoder/subdir.mk
-include CarOS/src/motor/DCmotor/driver/L298N/subdir.mk
-include CarOS/src/motor/DCmotor/driver/subdir.mk
-include CarOS/src/motor/DCmotor/controller/subdir.mk
-include CarOS/src/motor/DCmotor/subdir.mk
-include CarOS/src/memory/subdir.mk
-include CarOS/src/debug/subdir.mk
-include CarOS/src/communication/subdir.mk
-include CarOS/src/car/subdir.mk
-include CarOS/src/app/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STM32F407ZGT6_HAL_4WHEEL_MECANUM
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32F407ZGT6_HAL_4WHEEL_MECANUM.elf \

MAP_FILES += \
STM32F407ZGT6_HAL_4WHEEL_MECANUM.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32F407ZGT6_HAL_4WHEEL_MECANUM.list \


# All Target
all: main-build

# Main-build Target
main-build: STM32F407ZGT6_HAL_4WHEEL_MECANUM.elf secondary-outputs

# Tool invocations
STM32F407ZGT6_HAL_4WHEEL_MECANUM.elf STM32F407ZGT6_HAL_4WHEEL_MECANUM.map: $(OBJS) $(USER_OBJS) D:\Document\EE\C\ library\CarOS\v1.4\CarOS\demo\STM32F407ZGT6_HAL_4WHEEL_MECANUM\STM32F407ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STM32F407ZGT6_HAL_4WHEEL_MECANUM.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Document\EE\C library\CarOS\v1.4\CarOS\demo\STM32F407ZGT6_HAL_4WHEEL_MECANUM\STM32F407ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STM32F407ZGT6_HAL_4WHEEL_MECANUM.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32F407ZGT6_HAL_4WHEEL_MECANUM.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STM32F407ZGT6_HAL_4WHEEL_MECANUM.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32F407ZGT6_HAL_4WHEEL_MECANUM.elf STM32F407ZGT6_HAL_4WHEEL_MECANUM.list STM32F407ZGT6_HAL_4WHEEL_MECANUM.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
