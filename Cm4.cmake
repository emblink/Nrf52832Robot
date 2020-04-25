set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)
 
set(CMAKE_COLOR_MAKEFILE OFF) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
 
# specify the cross compiler
set(CMAKE_C_COMPILER "arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc.exe")
set(CMAKE_SIZE "arm-none-eabi-size.exe")
 
set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=softfp -fno-common -fmessage-length=0 -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS_DEBUG "${COMMON_FLAGS} -O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "${COMMON_FLAGS} -Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_FLAGS} -O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${COMMON_FLAGS} -Os -g0")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_EXE_LINKER_FLAGS "-lc -lgcc -mthumb -static --specs=nosys.specs --specs=nano.specs -Wl,--gc-sections -Wl,-Map=output.map")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")

