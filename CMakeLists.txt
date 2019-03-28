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
# 1394020320@qq.com
#

cmake_minimum_required(VERSION 3.4.3)
project(SOUI)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cotire)
include(MSVCMultipleProcessCompile)
include(SOUIOptions)

include(cmake/soui-demo.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

message(STATUS "COM_LIBS: ${COM_LIBS}")
message(STATUS "CORE_LIBS: ${CORE_LIBS}")
##
##
