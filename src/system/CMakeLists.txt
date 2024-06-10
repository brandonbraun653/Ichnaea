add_library(Ichnaea_System STATIC
  panic_handlers.cpp
  system_control.cpp
  system_error.cpp
)
target_link_libraries(Ichnaea_System PRIVATE Ichnaea_Headers Ichnaea_PicoLibs)