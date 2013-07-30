################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/c_ui.c \
../source/d_lcd.c \
../source/d_terminal.c 

OBJS += \
./source/c_ui.o \
./source/d_lcd.o \
./source/d_terminal.o 

C_DEPS += \
./source/c_ui.d \
./source/d_lcd.d \
./source/d_terminal.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DLinux_X86 -I"/home/al/projects/lms2012/lms2012/source" -I"/home/al/projects/lms2012/c_input/source" -I/usr/local/include -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


