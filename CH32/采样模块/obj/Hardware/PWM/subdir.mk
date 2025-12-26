################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/PWM/PWM.c 

C_DEPS += \
./Hardware/PWM/PWM.d 

OBJS += \
./Hardware/PWM/PWM.o 


EXPANDS += \
./Hardware/PWM/PWM.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
Hardware/PWM/%.o: ../Hardware/PWM/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"d:/WCH32/WCH32V203C8T6/采样模块/Debug" -I"d:/WCH32/WCH32V203C8T6/采样模块/Core" -I"d:/WCH32/WCH32V203C8T6/采样模块/User" -I"d:/WCH32/WCH32V203C8T6/采样模块/Peripheral/inc" -I"d:/WCH32/WCH32V203C8T6/采样模块/Hardware" -I"d:/WCH32/WCH32V203C8T6/采样模块/Hardware/OLED" -I"d:/WCH32/WCH32V203C8T6/采样模块/Hardware/PWM" -I"d:/WCH32/WCH32V203C8T6/采样模块/Hardware/ADC" -I"d:/WCH32/WCH32V203C8T6/采样模块/Hardware/ModBus" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

