################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/c_branch.c \
../source/c_compare.c \
../source/c_math.c \
../source/c_move.c \
../source/c_timer.c \
../source/lms2012.c \
../source/validate.c 

OBJS += \
./source/c_branch.o \
./source/c_compare.o \
./source/c_math.o \
./source/c_move.o \
./source/c_timer.o \
./source/lms2012.o \
./source/validate.o 

C_DEPS += \
./source/c_branch.d \
./source/c_compare.d \
./source/c_math.d \
./source/c_move.d \
./source/c_timer.d \
./source/lms2012.d \
./source/validate.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DLinux_X86 -I"/home/al/projects/lms2012/lms2012/source" -I"/home/al/projects/lms2012/c_input/source" -I"/home/al/projects/lms2012/c_output/source" -I"/home/al/projects/lms2012/c_memory/source" -I"/home/al/projects/lms2012/c_ui/source" -I"/home/al/projects/lms2012/c_com/source" -I"/home/al/projects/lms2012/c_sound/source" -I/usr/local/include -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


