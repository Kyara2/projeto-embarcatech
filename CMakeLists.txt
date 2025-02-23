cmake_minimum_required(VERSION 3.13...3.27)

# Inicializa o SDK baseado na variavel de ambiente PICO_SDK_PATH
# isso deve ser declarado antes do project(...)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(projeto_embarcatech	C CXX ASM)

# Initialize the Raspberry Pi Pico PICO_SDK_PATH
# Inicializa o microcontrolador Raspberry Pi Pico usando a variave de ambiente  PICO_SDK_PATH
pico_sdk_init()

add_executable(projeto_embarcatech
	projeto_embarcatech.c
)

pico_enable_stdio_usb(projeto_embarcatech 1)

# Adiciona as biblioteca necessarias
target_link_libraries(projeto_embarcatech
						pico_stdlib
						hardware_timer
						hardware_gpio
						hardware_pwm
					  )

# Cria o arquivo *.uf2 alem do *.elf
pico_add_extra_outputs(projeto_embarcatech)









