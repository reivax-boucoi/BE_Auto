################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ClockConfig.c \
../src/Cmd_moteur.c \
../src/ConfigADC.c \
../src/ConfigCTU.c \
../src/ConfigGPIO.c \
../src/ConfigPIT.c \
../src/ConfigPWM.c \
../src/Config_eDMA.c \
../src/Current_Sensor.c \
../src/DSPIConfig.c \
../src/MPC57xx__Interrupt_Init.c \
../src/ModeEntryAndConfig.c \
../src/UARTConfig.c \
../src/User_Interface.c \
../src/Vector.c \
../src/eTimer.c \
../src/flashrchw.c \
../src/gd3000.c \
../src/intc_SW_mode_isr_vectors_MPC5744P.c \
../src/main.c \
../src/modele_moteur_virtuel.c \
../src/pga411.c 

S_UPPER_SRCS += \
../src/intc_sw_handlers.S 

OBJS += \
./src/ClockConfig.o \
./src/Cmd_moteur.o \
./src/ConfigADC.o \
./src/ConfigCTU.o \
./src/ConfigGPIO.o \
./src/ConfigPIT.o \
./src/ConfigPWM.o \
./src/Config_eDMA.o \
./src/Current_Sensor.o \
./src/DSPIConfig.o \
./src/MPC57xx__Interrupt_Init.o \
./src/ModeEntryAndConfig.o \
./src/UARTConfig.o \
./src/User_Interface.o \
./src/Vector.o \
./src/eTimer.o \
./src/flashrchw.o \
./src/gd3000.o \
./src/intc_SW_mode_isr_vectors_MPC5744P.o \
./src/intc_sw_handlers.o \
./src/main.o \
./src/modele_moteur_virtuel.o \
./src/pga411.o 

C_DEPS += \
./src/ClockConfig.d \
./src/Cmd_moteur.d \
./src/ConfigADC.d \
./src/ConfigCTU.d \
./src/ConfigGPIO.d \
./src/ConfigPIT.d \
./src/ConfigPWM.d \
./src/Config_eDMA.d \
./src/Current_Sensor.d \
./src/DSPIConfig.d \
./src/MPC57xx__Interrupt_Init.d \
./src/ModeEntryAndConfig.d \
./src/UARTConfig.d \
./src/User_Interface.d \
./src/Vector.d \
./src/eTimer.d \
./src/flashrchw.d \
./src/gd3000.d \
./src/intc_SW_mode_isr_vectors_MPC5744P.d \
./src/main.d \
./src/modele_moteur_virtuel.d \
./src/pga411.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	powerpc-eabivle-gcc "@src/ClockConfig.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS Assembler'
	powerpc-eabivle-gcc "@src/intc_sw_handlers.args" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


