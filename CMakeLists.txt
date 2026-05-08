cmake_minimum_required(VERSION 3.16)

project(pidc_plasma_cpp
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(pidc_core
    src/Domain2D.cpp
)

target_include_directories(pidc_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(pidc_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(pidc_core PRIVATE /W4)
else()
    target_compile_options(pidc_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(pidc_smoke
    apps/smoke.cpp
)

target_link_libraries(pidc_smoke PRIVATE pidc_core)

enable_testing()

add_executable(pidc_test_periodic_boundary
    tests/test_periodic_boundary.cpp
)

target_link_libraries(pidc_test_periodic_boundary PRIVATE pidc_core)

add_test(
    NAME periodic_boundary
    COMMAND pidc_test_periodic_boundary
)

add_executable(pidc_test_shape_function_data
    tests/test_shape_function_data.cpp
)

target_link_libraries(pidc_test_shape_function_data PRIVATE pidc_core)

add_test(
    NAME shape_function_data
    COMMAND pidc_test_shape_function_data
)

add_executable(pidc_test_species
    tests/test_species.cpp
)

target_link_libraries(pidc_test_species PRIVATE pidc_core)

add_test(
    NAME species
    COMMAND pidc_test_species
)

add_executable(pidc_test_node_cloud
    tests/test_node_cloud.cpp
)

target_link_libraries(pidc_test_node_cloud PRIVATE pidc_core)

add_test(
    NAME node_cloud
    COMMAND pidc_test_node_cloud
)

add_executable(pidc_test_weight_function
    tests/test_weight_function.cpp
)

target_link_libraries(pidc_test_weight_function PRIVATE pidc_core)

add_test(
    NAME weight_function
    COMMAND pidc_test_weight_function
)

add_executable(pidc_test_polynomial_basis
    tests/test_polynomial_basis.cpp
)

target_link_libraries(pidc_test_polynomial_basis PRIVATE pidc_core)

add_test(
    NAME polynomial_basis
    COMMAND pidc_test_polynomial_basis
)
