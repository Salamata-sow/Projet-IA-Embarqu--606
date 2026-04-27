################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32N6xx_HAL_Driver/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := ETRS-606_project_AppliNonSecure
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
ETRS-606_project_AppliNonSecure.elf \

MAP_FILES += \
ETRS-606_project_AppliNonSecure.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
ETRS-606_project_AppliNonSecure.list \

OBJCOPY_BIN += \
ETRS-606_project_AppliNonSecure.bin \


# All Target
all: main-build

# Main-build Target
main-build: ETRS-606_project_AppliNonSecure.elf secondary-outputs

dependents:
	-cd D:/Users/user/Documents/tp2/ETRS-606_project/AppliSecure/Debug && $(MAKE) clean all

# Tool invocations
ETRS-606_project_AppliNonSecure.elf ETRS-606_project_AppliNonSecure.map: $(OBJS) $(USER_OBJS) D:/Users/user/Documents/tp2/ETRS-606_project/AppliSecure/Debug/ETRS-606_project_AppliSecure.elf D:\Users\user\Documents\tp2\ETRS-606_project\AppliNonSecure\STM32N657X0HXQ_LRUN.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "ETRS-606_project_AppliNonSecure.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m55 -T"D:\Users\user\Documents\tp2\ETRS-606_project\AppliNonSecure\STM32N657X0HXQ_LRUN.ld" --specs=nosys.specs -Wl,-Map="ETRS-606_project_AppliNonSecure.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

ETRS-606_project_AppliNonSecure.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "ETRS-606_project_AppliNonSecure.list"
	@echo 'Finished building: $@'
	@echo ' '

ETRS-606_project_AppliNonSecure.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "ETRS-606_project_AppliNonSecure.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) ETRS-606_project_AppliNonSecure.bin ETRS-606_project_AppliNonSecure.elf ETRS-606_project_AppliNonSecure.list ETRS-606_project_AppliNonSecure.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified
D:/Users/user/Documents/tp2/ETRS-606_project/AppliSecure/Debug/ETRS-606_project_AppliSecure.elf:

-include ../makefile.targets
