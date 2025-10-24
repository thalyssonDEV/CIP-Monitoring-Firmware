# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(main C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Defines the paths to the drivers
set(iolibrary_driver_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ioLibrary_Driver)
set(pico-ads1115_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers/pico-ads1115)

add_subdirectory(${pico-ads1115_SOURCE_DIR}/lib pico-ads1115-lib)

# Creates static library with necessary files from ioLibrary
add_library(iolibrary_static STATIC
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/wizchip_conf.c
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/socket.c
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/W5500/w5500.c
    ${iolibrary_driver_SOURCE_DIR}/Internet/DHCP/dhcp.c
    ${iolibrary_driver_SOURCE_DIR}/Internet/DNS/dns.c
    modules/ethernet_manager/w5500_config.c
)

# Adds ioLibrary include directories
target_include_directories(iolibrary_static PUBLIC
    ${iolibrary_driver_SOURCE_DIR}/Ethernet
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/W5500
    ${iolibrary_driver_SOURCE_DIR}/Internet/DHCP
    ${iolibrary_driver_SOURCE_DIR}/Internet/DNS
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/ethernet_manager
)

# Links with Pico SDK libraries
target_link_libraries(iolibrary_static
    pico_stdlib
    hardware_spi
)

# Define _WIZCHIP_ only - other macros defined in wizchip_macros.h
target_compile_definitions(iolibrary_static PRIVATE
    _WIZCHIP_=5500
)

# Carrega todas as variáveis do secrets.cmake para o build.
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/secrets.cmake)
    message(FATAL_ERROR "Arquivo 'secrets.cmake' nao encontrado!")
endif()
include(secrets.cmake)

# Carrega todas as variáveis do config.cmake para o build.
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/config.cmake)
    message(FATAL_ERROR "Arquivo 'config.cmake' nao encontrado!")
endif()
include(config.cmake)

# Add executable. Default name is the project name, version 0.1
add_executable(main src/main.c
modules/ethernet_manager/ethernet_manager.c
modules/ethernet_manager/w5500_config
modules/http_client/http_client.c
modules/sensor_manager/sensor_manager.c
modules/adc_manager/adc_manager.c
modules/analog_sensor/analog_sensor.c)

# Macros de pré-processador (-D) durante a compilação.
target_compile_definitions(main PRIVATE
    TARGET_SERVER_IP="${TARGET_SERVER_IP}"
    TARGET_PORT=${TARGET_PORT}
    TARGET_PATH="${TARGET_PATH}"
    HTTP_TIMEOUT_MS=${HTTP_TIMEOUT_MS}
    SENSOR_CONDUCTIVITY_MAX_VOLTAGE=${SENSOR_CONDUCTIVITY_MAX_VOLTAGE}
    SENSOR_CONDUCTIVITY_MAX_VALUE=${SENSOR_CONDUCTIVITY_MAX_VALUE}
    SENSOR_CONDUCTIVITY_MIN_VALUE=${SENSOR_CONDUCTIVITY_MIN_VALUE}
    SENSOR_TEMPERATURE_MAX_VOLTAGE=${SENSOR_TEMPERATURE_MAX_VOLTAGE}
    SENSOR_TEMPERATURE_MAX_VALUE=${SENSOR_TEMPERATURE_MAX_VALUE}
    SENSOR_TEMPERATURE_MIN_VALUE=${SENSOR_TEMPERATURE_MIN_VALUE}
    SENSOR_FLOW_MAX_VOLTAGE=${SENSOR_FLOW_MAX_VOLTAGE}
    SENSOR_FLOW_MAX_VALUE=${SENSOR_FLOW_MAX_VALUE}
    SENSOR_FLOW_MIN_VALUE=${SENSOR_FLOW_MIN_VALUE}
    CYCLE_INTERVAL_MS=${MAIN_TASK_CYCLE_INTERVAL_MS}
    WATCHDOG_TIMEOUT_MS=${SYSTEM_WATCHDOG_TIMEOUT_MS}
    ADC_CONNECTION_CHECK_TIMEOUT_MS=${ADC_CONNECTION_CHECK_TIMEOUT_MS}
    "BEARER_TOKEN=\"${BEARER_TOKEN}\""
)

# Itera sobre as listas de rede para criar as definições C necessárias
foreach(INDEX RANGE 5)
    list(GET ETHERNET_MAC ${INDEX} VALUE)
    target_compile_definitions(main PRIVATE "ETHERNET_MAC_${INDEX}=${VALUE}")
endforeach()

foreach(INDEX RANGE 3)
    list(GET DEVICE_IP ${INDEX} IP_VAL)
    list(GET GATEWAY_IP ${INDEX} GW_VAL)
    list(GET SUBNET_MASK ${INDEX} SN_VAL)
    target_compile_definitions(main PRIVATE 
        "DEVICE_IP_${INDEX}=${IP_VAL}"
        "GATEWAY_IP_${INDEX}=${GW_VAL}"
        "SUBNET_MASK_${INDEX}=${SN_VAL}"
    )
endforeach()

pico_set_program_name(main "main")
pico_set_program_version(main "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(main 0)
pico_enable_stdio_usb(main 1)

# Add the standard library to the build
target_link_libraries(main
        pico_stdlib
        hardware_i2c
        hardware_spi
        iolibrary_static
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
        pico-ads1115)

# Add the standard include files to the build
target_include_directories(main PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${iolibrary_driver_SOURCE_DIR}/Ethernet
        ${iolibrary_driver_SOURCE_DIR}/Ethernet/W5500
        ${iolibrary_driver_SOURCE_DIR}/Internet/DHCP
        ${iolibrary_driver_SOURCE_DIR}/Internet/DNS
        ${pico-ads1115_SOURCE_DIR}/lib
)

# Add any user requested libraries
target_link_libraries(main 
        pico_stdlib
        hardware_i2c
        hardware_spi
        iolibrary_static
        pico-ads1115
        )

pico_add_extra_outputs(main)