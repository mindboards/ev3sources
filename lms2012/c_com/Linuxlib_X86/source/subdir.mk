################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/c_bt.c \
../source/c_com.c \
../source/c_daisy.c \
../source/c_i2c.c \
../source/c_md5.c \
../source/c_wifi.c \
../source/os_internal.c \
../source/os_unix.c \
../source/wpa_ctrl.c 

OBJS += \
./source/c_bt.o \
./source/c_com.o \
./source/c_daisy.o \
./source/c_i2c.o \
./source/c_md5.o \
./source/c_wifi.o \
./source/os_internal.o \
./source/os_unix.o \
./source/wpa_ctrl.o 

C_DEPS += \
./source/c_bt.d \
./source/c_com.d \
./source/c_daisy.d \
./source/c_i2c.d \
./source/c_md5.d \
./source/c_wifi.d \
./source/os_internal.d \
./source/os_unix.d \
./source/wpa_ctrl.d 


# Each subdirectory must supply rules for building sources it contributes
source/c_bt.o: ../source/c_bt.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DLinux_X86 -I"/home/al/projects/lms2012/lms2012/source" -I/usr/local/include -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"source/c_bt.d" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/c_com.o: ../source/c_com.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DLinux_X86 -I"/home/al/projects/lms2012/lms2012/source" -I/usr/local/include -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"source/c_com.d" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -DLinux_X86 -I"/home/al/projects/lms2012/lms2012/source" -I/usr/local/include -I/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


