################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sockets/L2Socket.cpp \
../src/sockets/L3Socket.cpp \
../src/sockets/SwitchSocket.cpp 

OBJS += \
./src/sockets/L2Socket.o \
./src/sockets/L3Socket.o \
./src/sockets/SwitchSocket.o 

CPP_DEPS += \
./src/sockets/L2Socket.d \
./src/sockets/L3Socket.d \
./src/sockets/SwitchSocket.d 


# Each subdirectory must supply rules for building sources it contributes
src/sockets/%.o: ../src/sockets/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DBOOST_ALL_DYN_LINK -I../include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


