################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocol_handlers/ARPProtocolHandler.cpp \
../src/protocol_handlers/IPProtocolHandler.cpp 

OBJS += \
./src/protocol_handlers/ARPProtocolHandler.o \
./src/protocol_handlers/IPProtocolHandler.o 

CPP_DEPS += \
./src/protocol_handlers/ARPProtocolHandler.d \
./src/protocol_handlers/IPProtocolHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocol_handlers/%.o: ../src/protocol_handlers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DBOOST_ALL_DYN_LINK -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


