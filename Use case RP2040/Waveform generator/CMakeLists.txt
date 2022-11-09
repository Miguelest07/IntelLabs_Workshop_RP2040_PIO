# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(WaveformGenerator C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# ADD DIRECTORY
add_subdirectory(Includes)
add_subdirectory(pico-ssd1306)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    main.cpp
)

pico_generate_pio_header(${PROJECT_NAME}
        ${CMAKE_CURRENT_LIST_DIR}/pio_programs.pio
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib
    hardware_pio
    hardware_dma
    Includes
    hardware_adc
    hardware_i2c
    pico_ssd1306
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/Includes"
    "${PROJECT_SOURCE_DIR}/pico-ssd1306"
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)