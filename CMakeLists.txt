cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

# Define o nome do projeto como ProjetoIntegrador, suportando C, C++ e Assembly.
project(ProjetoIntegrador C CXX ASM) 
pico_sdk_init()


add_executable(${PROJECT_NAME}  
        ProjetoIntegrador.c
        inc/ssd1306.c
        )

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib 
        hardware_pio
        hardware_i2c
        hardware_pwm
        hardware_adc
        )

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)