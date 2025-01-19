project(IchnaeaTesting)

# Import CppUTest
add_subdirectory(${PROJECT_SOURCE_DIR}/../lib/cpputest ${CMAKE_CURRENT_BINARY_DIR}/cpputest)

# Include Dependent Headers
set(TESTING_INCLUDE_DIRECTORIES
  ${MBEDUTILS_TEST_EXPECT_DIR}
  ${MBEDUTILS_TEST_FAKE_DIR}
  ${MBEDUTILS_TEST_HARNESS_DIR}
  ${MBEDUTILS_TEST_MOCK_DIR}
  ${PROJECT_SOURCE_DIR}/../lib/cpputest/include
  ${PROJECT_SOURCE_DIR}/expect
  ${PROJECT_SOURCE_DIR}/mock
)

add_subdirectory(src/bsp)
add_subdirectory(src/hw/nor)
add_subdirectory(src/system/system_db)

# Add test targets
add_custom_target(BuildAllTests)
add_dependencies(BuildAllTests
  TestBoardMap
  TestNor
  TestSystemDB
)
