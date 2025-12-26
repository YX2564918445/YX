################################################################################
# MRS Version: 2.3.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Hardware/RELAY/subdir.mk
-include Hardware/OLED/subdir.mk
-include Hardware/MQ_2/subdir.mk
-include Hardware/LED/subdir.mk
-include Hardware/KEY/subdir.mk
-include Hardware/DS18B20/subdir.mk
-include Hardware/BEEP/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
工程技术综合实践.hex \

SECONDARY_LIST += \
工程技术综合实践.lst \

SECONDARY_SIZE += \
工程技术综合实践.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: 工程技术综合实践.elf secondary-outputs

# Tool invocations
工程技术综合实践.elf: $(OBJS) $(USER_OBJS_ESCAPE)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "d:/WCH32/WCH32V203C8T6/工程技术综合实践/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"工程技术综合实践.map" --specs=nano.specs --specs=nosys.specs -o "工程技术综合实践.elf" $(OBJS) $(LIBS)
工程技术综合实践.hex: 工程技术综合实践.elf
	@	riscv-none-embed-objcopy -O ihex "工程技术综合实践.elf" "工程技术综合实践.hex"
工程技术综合实践.lst: 工程技术综合实践.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "工程技术综合实践.elf" > "工程技术综合实践.lst"
工程技术综合实践.siz: 工程技术综合实践.elf
	riscv-none-embed-size --format=berkeley "工程技术综合实践.elf"

# Other Targets
clean:
	-$(RM) $(DIR_OBJS) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE) 工程技术综合实践.elf
	-$(RM) $(DIR_EXPANDS) $(CALLGRAPH_DOT)
	-$(RM) $(DIR_DEPS)
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets