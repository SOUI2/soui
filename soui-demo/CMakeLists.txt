
cmake_minimum_required(VERSION 3.4.3)
project(SOUI)


file(GLOB demos "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach (folder IN LISTS demos)
    file(GLOB T "${folder}/CMakeLists.txt")
    if (T)
        message("add_subdirectory: ${folder}")
        add_subdirectory(${folder})
    endif ()
endforeach (folder)
