################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
radio/radio.obj: ../radio/radio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/TivaWare_C_Series-2.1.2.111" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.0.LTS/include" --include_path="C:/Users/mengfanli2/workspace_2_1/car_radio_echo/motor_ctl" --include_path="C:/Users/mengfanli2/workspace_2_1/car_radio_echo/radio" --include_path="C:/Users/mengfanli2/workspace_2_1/car_radio_echo/bee" -g --c99 --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="radio/radio.d" --obj_directory="radio" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


