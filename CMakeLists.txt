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

# Download WIZnet ioLibrary_Driver
include(FetchContent)

FetchContent_Declare(
    iolibrary_driver
    GIT_REPOSITORY https://github.com/Wiznet/ioLibrary_Driver.git
    GIT_TAG master
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers/ioLibrary_Driver
)
FetchContent_MakeAvailable(iolibrary_driver)

# Download PICO-ADS1115 Driver
FetchContent_Declare(
    pico-ads1115
    GIT_REPOSITORY https://github.com/antgon/pico-ads1115.git
    GIT_TAG 2541304300f7591eec2bacd9ff337505efa20331
)
FetchContent_MakeAvailable(pico-ads1115)

add_subdirectory(${pico-ads1115_SOURCE_DIR}/lib pico-ads1115-lib)

# Cria biblioteca estática com os arquivos necessários da ioLibrary
add_library(iolibrary_static STATIC
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/wizchip_conf.c
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/socket.c
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/W5500/w5500.c
    ${iolibrary_driver_SOURCE_DIR}/Internet/DHCP/dhcp.c
    ${iolibrary_driver_SOURCE_DIR}/Internet/DNS/dns.c
    modules/ethernet_manager/w5500_config.c
)

# Adiciona diretórios de include da ioLibrary
target_include_directories(iolibrary_static PUBLIC
    ${iolibrary_driver_SOURCE_DIR}/Ethernet
    ${iolibrary_driver_SOURCE_DIR}/Ethernet/W5500
    ${iolibrary_driver_SOURCE_DIR}/Internet/DHCP
    ${iolibrary_driver_SOURCE_DIR}/Internet/DNS
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/ethernet_manager
)

# Linka com as bibliotecas do Pico SDK
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
    message(FATAL_ERROR "Arquivo 'secrets.cmake' nao encontrado! Por favor, crie um a partir do 'secrets.cmake.example'.")
endif()
include(secrets.cmake)

# Add executable. Default name is the project name, version 0.1

add_executable(main src/main.c
modules/temperature/temperature.c
modules/concentration/concentration.c
modules/flow/flow.c
modules/ethernet_manager/ethernet_manager.c
modules/ethernet_manager/w5500_config
modules/http_client/http_client.c
modules/sensor_manager/sensor_manager.c
modules/adc_manager/adc_manager.c)

# Macros de pré-processador (-D) durante a compilação.
target_compile_definitions(main PRIVATE
    TARGET_SERVER_IP="${TARGET_SERVER_IP}"
    TARGET_PORT=${TARGET_PORT}
    TARGET_PATH="${TARGET_PATH}"
    ETHERNET_MAC_0=0x00
    ETHERNET_MAC_1=0x08
    ETHERNET_MAC_2=0xDC
    ETHERNET_MAC_3=0x11
    ETHERNET_MAC_4=0x22
    ETHERNET_MAC_5=0x33
    DEVICE_IP_0=192
    DEVICE_IP_1=168
    DEVICE_IP_2=15
    DEVICE_IP_3=2
    GATEWAY_IP_0=192
    GATEWAY_IP_1=168
    GATEWAY_IP_2=15
    GATEWAY_IP_3=1
    SUBNET_MASK_0=255
    SUBNET_MASK_1=255
    SUBNET_MASK_2=255
    SUBNET_MASK_3=0

    # NÃO MEXER NAS VARIÁVEIS ACIMA, SENÃO NAO FUNCIONA.
)

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