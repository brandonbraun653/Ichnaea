
add_executable(Ichnaea
        main.cpp
        )

# pull in common dependencies
target_link_libraries(Ichnaea
        pico_stdlib
        hardware_gpio
)

# create map/bin/hex file etc.
pico_add_extra_outputs(Ichnaea)