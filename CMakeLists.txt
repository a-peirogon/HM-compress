cmake_minimum_required(VERSION 3.16)
project(hm_compress
    VERSION 1.0.0
    DESCRIPTION "Modeling mathematics via compression in monoids (Aksenov, Bodnia, Freedman, Mulligan)"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -fsanitize=address,undefined")

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

include(FetchContent)

add_library(hmcompress
    src/monoid/free_abelian_monoid.cpp
    src/monoid/free_monoid.cpp
    src/macros/macro_set.cpp
    src/expansion/expansion_function.cpp
    src/expansion/coin_dp.cpp
    src/expansion/segmentation_dp.cpp
    src/statistics/linear_regression.cpp
    src/utils/bigint.cpp
)

target_include_directories(hmcompress
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(hmcompress PUBLIC cxx_std_20)

if(BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4
    )
    FetchContent_MakeAvailable(catch2)

    add_executable(hmcompress_tests
        tests/test_free_abelian_monoid.cpp
        tests/test_free_monoid.cpp
        tests/test_macro_set.cpp
        tests/test_expansion_theorems.cpp
        tests/test_coin_dp.cpp
        tests/test_segmentation_dp.cpp
        tests/test_bigint.cpp
        tests/test_linear_regression.cpp
    )
    target_link_libraries(hmcompress_tests PRIVATE hmcompress Catch2::Catch2WithMain)

    include(CTest)
    include(Catch)
    catch_discover_tests(hmcompress_tests)
endif()

if(BUILD_EXAMPLES)
    add_executable(place_notation_demo examples/place_notation_demo.cpp)
    target_link_libraries(place_notation_demo PRIVATE hmcompress)

    add_executable(waring_expansion_demo examples/waring_expansion_demo.cpp)
    target_link_libraries(waring_expansion_demo PRIVATE hmcompress)

    add_executable(free_monoid_segmentation_demo examples/free_monoid_segmentation_demo.cpp)
    target_link_libraries(free_monoid_segmentation_demo PRIVATE hmcompress)

    add_executable(theorem1_expansion_curve examples/theorem1_expansion_curve.cpp)
    target_link_libraries(theorem1_expansion_curve PRIVATE hmcompress)
endif()

add_executable(hmcompress_main src/main.cpp)
target_link_libraries(hmcompress_main PRIVATE hmcompress)
