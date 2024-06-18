add_library(Ichnaea_Board STATIC
  board_map.cpp
)
target_link_libraries(Ichnaea_Board PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)