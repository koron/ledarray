# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(ledarray C CXX ASM)

# Initialise the Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(ledarray
	main.c
	ledarray.c
	)

pico_generate_pio_header(ledarray
	${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
	OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

#pico_generate_pio_header(ledarray
#	${CMAKE_CURRENT_LIST_DIR}/ws2812dma.pio
#	OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# Link Time Optimization is not supported yet.
#set_property(TARGET matrixscan PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

pico_set_program_name(ledarray "ledarray")
pico_set_program_version(ledarray "0.1")

pico_enable_stdio_uart(ledarray 1)
pico_enable_stdio_usb(ledarray 0)

target_link_libraries(ledarray
	pico_stdlib
        hardware_pio
	hardware_dma
	)

pico_add_extra_outputs(ledarray)
