cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ultrasonic_demo_project)

pico_sdk_init()

add_executable(ultrasonic_demo
        ultrasonic_demo.c ultrasonic.c timer.c
        )

# pull in common dependencies
target_link_libraries(
        ultrasonic_demo 
        pico_stdlib
        pico_bootsel_via_double_reset
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(ultrasonic_demo)

