
# Defines pre_project_set_up_hermetic_build() and set_up_hermetic_build().
include(cmake/hermetic_build.cmake OPTIONAL)

if (COMMAND pre_project_set_up_hermetic_build)
  pre_project_set_up_hermetic_build()
endif()

########################################################################
#
# Project-wide settings

# Name of the project.
#
# CMake files in this project can refer to the root source directory
# as ${gtest_SOURCE_DIR} and to the root binary directory as
# ${gtest_BINARY_DIR}.
# Language "C" is required for find_package(Threads).
project(gtest CXX C)

if (COMMAND set_up_hermetic_build)
  set_up_hermetic_build()
endif()

# Where Google Test's .h files can be found.
include_directories(
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR})

########################################################################
#SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")# 直接抄gui的默认，然后 改成mtd
#SET(CMAKE_CXX_FLAGS_RELEASE "/MD /O2 /Ob2 /D NDEBUG")
add_library(gtest STATIC src/gtest-all.cc src/gtest_main.cc)
set_target_properties (gtest PROPERTIES
    FOLDER third-part
)
cotire(gtest)

SOUI_INSTALL_TARGETS(gtest)
