cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_TOOLCHAIN_FILE "Cm4.cmake")
set (LANGUAGES C ASM)

project("NRF_Robot" ${LANGUAGES})

add_definitions(
    -DNRF52832_XXAA
    -DCONFIG_NFCT_PINS_AS_GPIOS
    -DCONFIG_GPIO_AS_PINRESET
    -DFLOAT_ABI_HARD
)

set(APP_INCLUDES
    system
    src
)

set(APP_SRCS
    src/main.c
    src/clock.c
    src/gpio.c
    src/temp.c
    src/timer.c
    src/stepMotorDriver.c
    src/pinInterrupt.c
    src/systemTime.c
    src/twim.c
    src/mpu9250.c
)

set(SDK_SRCS
    system/system_nrf52.c
    system/gcc_startup_nrf52.S
)

set(LINKER_SCRIPT
    -T${PROJECT_SOURCE_DIR}/linker/nrf52832_xxab.ld
)

set(LINK_DIRS

)

set(LINK_LIBS

)

include_directories(${APP_INCLUDES})
link_directories(${LINK_DIRS})
add_executable(${PROJECT_NAME} ${APP_SRCS} ${SDK_SRCS})
target_link_options(${PROJECT_NAME} PRIVATE ${LINKER_SCRIPT})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

add_custom_command(TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
	COMMAND ${CMAKE_SIZE} ARGS ${PROJECT_NAME}.elf
)
