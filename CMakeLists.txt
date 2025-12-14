# CMakeLsts.txt - cmake for microcontrollers
# Copyright (C) 2025 Camren Chraplak

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# esp-idf for esp32
if(DEFINED IDF_TARGET)

    idf_component_register(
        SRC_DIRS "src" "src/esp32" "src/private" "src/test_hardware_timer" "src/test_hardware_timer/test_print"
        INCLUDE_DIRS "src"
        REQUIRES driver esp_timer unity
    )

    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-expansion-to-defined)

# pico sdk for rpi pico
elseif(DEFINED PICO_BOARD)

    add_library(universal_hardware_timer INTERFACE)

    target_sources(universal_hardware_timer INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pico/board_pico_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/private/hardware_timer_priv.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test_hardware_timer/hardware_timer_test_delay.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test_hardware_timer/hardware_timer_test_priv.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test_hardware_timer/test_print/hardware_timer_print_printf.c
    )

    target_include_directories(universal_hardware_timer INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/src
    )

endif()