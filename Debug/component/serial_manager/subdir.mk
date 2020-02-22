################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/serial_manager/serial_manager.c \
../component/serial_manager/serial_port_uart.c 

OBJS += \
./component/serial_manager/serial_manager.o \
./component/serial_manager/serial_port_uart.o 

C_DEPS += \
./component/serial_manager/serial_manager.d \
./component/serial_manager/serial_port_uart.d 


# Each subdirectory must supply rules for building sources it contributes
component/serial_manager/%.o: ../component/serial_manager/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/board" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/source" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/drivers" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/device" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/CMSIS" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/component/uart" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/utilities" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/component/serial_manager" -I"/home/leandro/MEGA/Dron/FreedomProjects/Quadcopter/component/lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


