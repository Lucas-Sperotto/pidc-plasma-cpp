cmake_minimum_required(VERSION 3.16)

project(pidc_plasma_cpp
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_library(pidc_core
    src/Domain2D.cpp
)

target_include_directories(pidc_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(pidc_core PUBLIC cxx_std_17)

target_link_libraries(pidc_core
    PUBLIC
        Eigen3::Eigen
)

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

add_executable(pidc_test_eigen_dependency
    tests/test_eigen_dependency.cpp
)

target_link_libraries(pidc_test_eigen_dependency PRIVATE pidc_core)

add_test(
    NAME eigen_dependency
    COMMAND pidc_test_eigen_dependency
)

add_executable(pidc_test_mls_shape_function
    tests/test_mls_shape_function.cpp
)

target_link_libraries(pidc_test_mls_shape_function PRIVATE pidc_core)

add_test(
    NAME mls_shape_function
    COMMAND pidc_test_mls_shape_function
)

add_executable(pidc_test_mls_robustness
    tests/test_mls_robustness.cpp
)

target_link_libraries(pidc_test_mls_robustness PRIVATE pidc_core)

add_test(
    NAME mls_robustness
    COMMAND pidc_test_mls_robustness
)

add_executable(pidc_test_regular_node_cloud
    tests/test_regular_node_cloud.cpp
)

target_link_libraries(pidc_test_regular_node_cloud PRIVATE pidc_core)

add_test(
    NAME regular_node_cloud
    COMMAND pidc_test_regular_node_cloud
)

add_executable(pidc_test_neighbor_search_grid
    tests/test_neighbor_search_grid.cpp
)

target_link_libraries(pidc_test_neighbor_search_grid PRIVATE pidc_core)

add_test(
    NAME neighbor_search_grid
    COMMAND pidc_test_neighbor_search_grid
)

add_executable(pidc_test_periodic_boundary2d
    tests/test_periodic_boundary2d.cpp
)

target_link_libraries(pidc_test_periodic_boundary2d PRIVATE pidc_core)

add_test(
    NAME periodic_boundary2d
    COMMAND pidc_test_periodic_boundary2d
)

add_executable(pidc_test_gauss_cell2d
    tests/test_gauss_cell2d.cpp
)

target_link_libraries(pidc_test_gauss_cell2d PRIVATE pidc_core)

add_test(
    NAME gauss_cell2d
    COMMAND pidc_test_gauss_cell2d
)

add_executable(pidc_test_efg_poisson_mms
    tests/test_efg_poisson_mms.cpp
)

target_link_libraries(pidc_test_efg_poisson_mms PRIVATE pidc_core)

add_test(
    NAME efg_poisson_mms
    COMMAND pidc_test_efg_poisson_mms
)

add_executable(pidc_test_charge_conservation
    tests/test_charge_conservation.cpp
)

target_link_libraries(pidc_test_charge_conservation PRIVATE pidc_core)

add_test(
    NAME charge_conservation
    COMMAND pidc_test_charge_conservation
)

add_executable(pidc_test_pic_grid1d
    tests/test_pic_grid1d.cpp
)

target_link_libraries(pidc_test_pic_grid1d PRIVATE pidc_core)

add_test(
    NAME pic_grid1d
    COMMAND pidc_test_pic_grid1d
)

add_executable(pidc_test_cic_deposition_1d
    tests/test_cic_deposition_1d.cpp
)

target_link_libraries(pidc_test_cic_deposition_1d PRIVATE pidc_core)

add_test(
    NAME cic_deposition_1d
    COMMAND pidc_test_cic_deposition_1d
)

add_executable(pidc_test_manufactured_field_1d
    tests/test_manufactured_field_1d.cpp
)

target_link_libraries(pidc_test_manufactured_field_1d PRIVATE pidc_core)

add_test(
    NAME manufactured_field_1d
    COMMAND pidc_test_manufactured_field_1d
)

add_executable(pidc_test_poisson_solver_1d
    tests/test_poisson_solver_1d.cpp
)

target_link_libraries(pidc_test_poisson_solver_1d PRIVATE pidc_core)

add_test(
    NAME poisson_solver_1d
    COMMAND pidc_test_poisson_solver_1d
)
