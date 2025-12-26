################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../Startup/startup_ch32v20x_D6.S 

S_UPPER_DEPS += \
./Startup/startup_ch32v20x_D6.d 

OBJS += \
./Startup/startup_ch32v20x_D6.o 


EXPANDS += \
./Startup/startup_ch32v20x_D6.S.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
Startup/%.o: ../Startup/%.S
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -x assembler-with-cpp -I"d:/WCH32/WCH32V203C8T6/²ÉÑùÄ£¿é/Startup" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

