################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/adc.c \
../Src/delay.c \
../Src/i2c.c \
../Src/lcd.c \
../Src/main.c \
../Src/rtc.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/utils.c 

OBJS += \
./Src/adc.o \
./Src/delay.o \
./Src/i2c.o \
./Src/lcd.o \
./Src/main.o \
./Src/rtc.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/utils.o 

C_DEPS += \
./Src/adc.d \
./Src/delay.d \
./Src/i2c.d \
./Src/lcd.d \
./Src/main.d \
./Src/rtc.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -DSTM32F446xx -c -I../Inc -I"C:/stm32_training/chip_headers/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/stm32_training/chip_headers/Drivers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/adc.cyclo ./Src/adc.d ./Src/adc.o ./Src/adc.su ./Src/delay.cyclo ./Src/delay.d ./Src/delay.o ./Src/delay.su ./Src/i2c.cyclo ./Src/i2c.d ./Src/i2c.o ./Src/i2c.su ./Src/lcd.cyclo ./Src/lcd.d ./Src/lcd.o ./Src/lcd.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/rtc.cyclo ./Src/rtc.d ./Src/rtc.o ./Src/rtc.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/utils.cyclo ./Src/utils.d ./Src/utils.o ./Src/utils.su

.PHONY: clean-Src

