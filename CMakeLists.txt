cmake_minimum_required(VERSION 3.16)

project(BOUNDED_BLOCK_LENGTH VERSION 0.0.0)

add_library(bounded MODULE dsp.cpp)
set_target_properties(bounded PROPERTIES PREFIX "" SUFFIX "")
target_compile_features(bounded PRIVATE cxx_std_17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LV2 REQUIRED IMPORTED_TARGET lv2)

target_link_libraries(bounded PRIVATE PkgConfig::LV2)

