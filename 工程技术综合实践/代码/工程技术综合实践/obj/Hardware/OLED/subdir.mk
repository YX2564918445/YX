################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hardware/OLED/OLED.c 

C_DEPS += \
./Hardware/OLED/OLED.d 

OBJS += \
./Hardware/OLED/OLED.o 

DIR_OBJS += \
./Hardware/OLED/*.o \

DIR_DEPS += \
./Hardware/OLED/*.d \

DIR_EXPANDS += \
./Hardware/OLED/*.234r.expand \


# Each subdirectory must supply rules for building sources it contributes
Hardware/OLED/%.o: ../Hardware/OLED/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Debug" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Core" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/User" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Peripheral/inc" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/OLED" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/BEEP" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/RELAY" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/LED" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/DS18B20" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/KEY" -I"d:/WCH32/WCH32V203C8T6/工程技术综合实践/Hardware/MQ_2" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

