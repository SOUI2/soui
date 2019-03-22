#
# SOUI cmake配置文件
# 生成主要的SOUI库和demo
# 使用帮助
#
# 使用cmake 3.4.3 或以上版本
# 
#
# 目前支持编译的库/可执行程序:
# 
# ---third-part
# third-part/png
# third-part/zlib
# third-part/skia
# third-part/png
# third-part/lua
# 
# ---core---
# SOUI
# utilities
# soui-sys-resource
#
# ---components---
# components/imagedecoder-gdip
# components/imagedecoder-png
# components/imagedecoder-stb
# components/imagedecoder-wic
# components/render-gdi
# components/render-skia
# components/resprovider-zip
# components/translator
# components/ScriptMode-Lua
# components/log4z
#
# ---demos---
# demos/360
# demos/360Preview
# demos/PCManager
#
# __cmake/cotire.cmake
# @ https://github.com/sakra/cotire
# 预处理头文件支持
#
# __cmake/internal_utils.cmake 
# 内置工具模块
#
#
# mkdir build
# cd build 
# cmake .. -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
# nmake
#
# or 
# cmake .. -G"Visual Studio 14 2015"
# 
# 生成SOUI.sln工程文件
#   
# options:
#   SHARED_CRT             ON 
#   USE_UNICODE            ON
#   WCHAR_AS_DEFAULT       ON 
#   XP_TOOLSET             ON   # visual studio 2012
#   ENABLE_SOUI_CORE_LIB   OFF 
#   ENABLE_SOUI_COM_LIB    OFF
# 
# 1394020320@qq.com
#


option(USE_UNICODE "Use Unicode" ON)

# When other libraries are using a shared version of runtime libraries,
# Google Test also has to use one.
option(SHARED_CRT "Use shared crt runtime library." ON)

# When other libraries are using a shared version of runtime libraries,
# Google Test also has to use one.
option(WCHAR_AS_DEFAULT "Use wchar_t as internal type" ON)

#
#
option(XP_TOOLSET "" ON)

#
#
option(ENABLE_SOUI_CORE_LIB "Enable compile 'core' as static lib" OFF)
#
#
option(ENABLE_SOUI_COM_LIB "Enable compile 'components' as static lib" OFF)

option(OUTPATH_WITHOUT_TYPE "Put All generation in same Path" ON)

option(DOWNLOAD_COMPILE_DEMOS "下载并编译Soui-demo中的demo" OFF)
#
#
#
cmake_minimum_required(VERSION 3.4.3)
project(SOUI)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/__cmake/")

#
# 设置输出目录
#
if (OUTPATH_WITHOUT_TYPE)
    message("---Put All generation in same Path")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)
else()
    message("---Put All generation in different Path")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
endif()

#
# 设置多线程编译
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /wd4100 /wd4189 /wd4996")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /wd4100 /wd4189 /wd4996")

#
# 设置输入文件的后缀
#
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "possible configurations" FORCE)

include(__cmake/cotire.cmake)
include(__cmake/internal_utils.cmake)
config_compiler_and_linker()

if (ENABLE_SOUI_CORE_LIB)
    message("---Building [soui] with LIB_CORE")
else()
    message("---Building [soui] with DLL_CORE")
endif()
if (ENABLE_SOUI_COM_LIB)
    message("---Building [soui components] with LIB_SOUI_COM")
else()
    message("---Building [soui components] with DLL_SOUI_COM")
endif()

set(CORE_LIBS CACHE INTERNAL "core_lib")
set(COM_LIBS CACHE INTERNAL "com_lib")

#
#
configure_file("config/core-def.h.in" "${PROJECT_SOURCE_DIR}/config/core-def.h" @ONLY)
configure_file("config/com-def.h.in" "${PROJECT_SOURCE_DIR}/config/com-def.h" @ONLY)
configure_file("config/build.cfg.in" "${PROJECT_SOURCE_DIR}/config/build.cfg" @ONLY)

#
#
add_subdirectory(third-part)

#
#
#
add_subdirectory(utilities)
add_subdirectory(SOUI)
add_subdirectory(soui-sys-resource)

#
#
# 
add_subdirectory(components)

#
#
add_subdirectory(demo)
include(__cmake/soui-demo.cmake)

# Add SOUI Demos
add_subdirectory(soui-demo)

message(STATUS "COM_LIBS: ${COM_LIBS}")
message(STATUS "CORE_LIBS: ${CORE_LIBS}")
##
##