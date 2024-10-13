include(test_target.cmake)

project(IchnaeaTesting)

# Import CppUTest
add_subdirectory(${PROJECT_SOURCE_DIR}/../lib/cpputest ${CMAKE_CURRENT_BINARY_DIR}/cpputest)

# Include Dependent Headers
set(TESTING_INCLUDE_DIRECTORIES
    ${PROJECT_SOURCE_DIR}/../lib/cpputest/include
    # ${PROJECT_SOURCE_DIR}/../lib/etl/include
    # ${PROJECT_SOURCE_DIR}/expect
    # ${PROJECT_SOURCE_DIR}/fake
    # ${PROJECT_SOURCE_DIR}/harness
    # ${PROJECT_SOURCE_DIR}/mock
)

add_subdirectory(src/bsp)

# Add test targets
add_custom_target(BuildAllTests)
add_dependencies(BuildAllTests
  TestBoardMap
)
