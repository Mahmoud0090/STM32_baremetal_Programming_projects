################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
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


BUILD_ARTIFACT_NAME := Weather Station with LCD Display_and_RTC
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Weather\ Station\ with\ LCD\ Display_and_RTC.elf \

MAP_FILES += \
Weather\ Station\ with\ LCD\ Display_and_RTC.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Weather\ Station\ with\ LCD\ Display_and_RTC.list \


# All Target
all: main-build

# Main-build Target
main-build: Weather\ Station\ with\ LCD\ Display_and_RTC.elf secondary-outputs

# Tool invocations
Weather\ Station\ with\ LCD\ Display_and_RTC.elf Weather\ Station\ with\ LCD\ Display_and_RTC.map: $(OBJS) $(USER_OBJS) C:\stm32_training\Weather\ Station\ with\ LCD\ Display_and_RTC\STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Weather Station with LCD Display_and_RTC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\stm32_training\Weather Station with LCD Display_and_RTC\STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Weather Station with LCD Display_and_RTC.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Weather\ Station\ with\ LCD\ Display_and_RTC.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Weather Station with LCD Display_and_RTC.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Weather\ Station\ with\ LCD\ Display_and_RTC.elf Weather\ Station\ with\ LCD\ Display_and_RTC.list Weather\ Station\ with\ LCD\ Display_and_RTC.map Weather\\ Station\\ with\\ LCD\\ Display_and_RTC.elf default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
