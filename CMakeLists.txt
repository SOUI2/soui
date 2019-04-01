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
# k000<AtaLuZiK@gmail.com>

cmake_minimum_required(VERSION 3.4.3)
project(SOUI)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(SOUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include(cotire)
include(MSVCMultipleProcessCompile)
include(SOUIHelpers)
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


# 此处不使用 CMakePackageConfigHelpers 导出包，仅生成相关的目标导入文件。
# 不推荐使用以下方式进行集成，因为没有明确的依赖关系，需要提前编译好所需要的依
# 赖，需要自行拷贝依赖的动态链接库至运行目录下，实际生产环境下重用性低。
# 在单一小项目中推荐将`SOUI`克隆至项目目录下，使用`add_subdirectory`添加`SOUI`
# 为子项目。
# 需要完备包管理请自行结合[Conan](https://conan.io/)或`vcpkg`完成。
#
# 使用方法：
# 外部项目中调用`cmake .. -DSOUI_MODULE_PATH=SOUI的CMake输出目录`

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SOUIConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/SOUIConfig.cmake)
file(
  COPY
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cotire.cmake
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/cmake)
export(EXPORT SOUITargets)
