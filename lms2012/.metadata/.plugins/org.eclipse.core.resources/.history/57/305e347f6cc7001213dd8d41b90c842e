################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include source/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: lms2012

# Tool invocations
lms2012: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	~/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-gcc -L"/home/al/projects/lms2012/lms2012/Linux_AM1808/sys/lib" -L"/home/al/am1808/DaVinci-PSP-SDK-03.20.00.13/linux-devkit/arm-none-linux-gnueabi/usr/lib" -L~/CodeSourcery/Sourcery_G++_Lite/arm-none-linux-gnueabi/lib -o"lms2012" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) lms2012
	-@echo ' '

post-build:
	-cp /home/al/projects/lms2012/lms2012/Linux_AM1808/lms2012 /home/al/projects/lms2012/lms2012/Linux_AM1808/sys/lms2012;cd /home/al/projects/lms2012/open_first;./am1808_run
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
