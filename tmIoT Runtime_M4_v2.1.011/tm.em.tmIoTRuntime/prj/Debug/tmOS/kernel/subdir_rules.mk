################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
tmOS/kernel/app.obj: C:/Users/Administrator/Desktop/tmIoT\ Runtime_M4_v2.1.011/tm.em.tmOS_M4/kernel/src/app.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.tmIoTRuntime/app/include" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.comm_lib_M4" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.tmOS_M4" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294KCPDT --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="tmOS/kernel/app.d" --obj_directory="tmOS/kernel" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tmOS/kernel/device.obj: C:/Users/Administrator/Desktop/tmIoT\ Runtime_M4_v2.1.011/tm.em.tmOS_M4/kernel/src/device.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.tmIoTRuntime/app/include" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.comm_lib_M4" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.tmOS_M4" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294KCPDT --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="tmOS/kernel/device.d" --obj_directory="tmOS/kernel" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

tmOS/kernel/task.obj: C:/Users/Administrator/Desktop/tmIoT\ Runtime_M4_v2.1.011/tm.em.tmOS_M4/kernel/src/task.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.tmIoTRuntime/app/include" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.comm_lib_M4" --include_path="C:/Users/Administrator/Desktop/tmIoT Runtime_M4_v2.1.011/tm.em.tmOS_M4" -g --gcc --define=ccs="ccs" --define=TARGET_IS_TM4C129_RA0 --define=PART_TM4C1294KCPDT --diag_wrap=off --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="tmOS/kernel/task.d" --obj_directory="tmOS/kernel" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

