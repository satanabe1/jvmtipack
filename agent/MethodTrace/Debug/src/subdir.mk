################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AgentSeed.cpp 

C_SRCS += \
../src/oldMethodTrace.c 

OBJS += \
./src/AgentSeed.o \
./src/oldMethodTrace.o 

C_DEPS += \
./src/oldMethodTrace.d 

CPP_DEPS += \
./src/AgentSeed.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -shared -I"/Users/wtnbsts/Dropbox/jvmtipack/agent/Header/darwin" -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc  -shared -I/Users/wtnbsts/Dropbox/jvmtipack/agent/Header/darwin -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


