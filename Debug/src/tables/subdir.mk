################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/tables/ARPTable.cpp \
../src/tables/RoutingTable.cpp 

OBJS += \
./src/tables/ARPTable.o \
./src/tables/RoutingTable.o 

CPP_DEPS += \
./src/tables/ARPTable.d \
./src/tables/RoutingTable.d 


# Each subdirectory must supply rules for building sources it contributes
src/tables/%.o: ../src/tables/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DBOOST_ALL_DYN_LINK -I"/home/cfv/git/L2L3Forwarder/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


