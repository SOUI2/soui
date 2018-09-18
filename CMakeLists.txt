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
# lijinggang@021.com
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
option(XP_TOOLSET "" , ON)

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

#
# 下载并编译Soui-demo中的demo
#
if (DOWNLOAD_COMPILE_DEMOS)
    message("---下载并编译Soui-demo中的demo")
    find_package(Git)
    if(Git_FOUND)
        message("Git found: ${GIT_EXECUTABLE}")
        message("Git clone TimDemo, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/TimDemo.git")
        message("Git clone UiEditor, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/UiEditor.git")
        message("Git clone qqlogin, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/qqlogin.git")
        message("Git clone 360, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/360.git")
        message("Git clone 360Preview, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/360Preview.git")
        message("Git clone souispy, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/souispy.git")
        message("Git clone PcManager, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/PcManager.git")
        message("Git clone SoTool, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/SoTool.git")
        message("Git clone mclistview_demo, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/mclistview_demo.git")
        message("Git clone souitest, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/souitest.git")
        message("Git clone VUI, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/VUI.git")
        message("Git clone BesLyric, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/BesLyric.git")
        message("Git clone QQMain, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/QQMain.git")
        message("Git clone FrogPlay, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/FrogPlay.git")
        message("Git clone MultiLangs, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/MultiLangs.git")
        message("Git clone TXMgr, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/TXMgr.git")
        message("Git clone SLogViewer, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/SLogViewer.git")
        message("Git clone souieditor, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/souieditor.git")
        message("Git clone DropDemo, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/DropDemo.git")
        message("Git clone ThreeClearGame, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/ThreeClearGame.git")
        message("Git clone baiduwp, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/baiduwp.git")
        message("Git clone ScreenShotTool, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/ScreenShotTool.git")
        message("Git clone SoftMaster, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/SoftMaster.git")
        message("Git clone SweepingEdge, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/SweepingEdge.git")
        message("Git clone iectrl, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/iectrl.git")
        message("Git clone mfc.demo, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/mfc.demo.git")
        message("Git clone CapScreenDemo, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/CapScreenDemo.git")
        message("Git clone SOUI-CEF, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/SOUI-CEF.git")
        message("Git clone nsisdemo, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui-demo/nsisdemo.git")
        message("Git clone Complete")
        FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/__cmake/CMakeLists.txt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/demos/)
        ADD_SUBDIRECTORY(demos)
    endif()
endif()

message(STATUS "COM_LIBS: ${COM_LIBS}")
message(STATUS "CORE_LIBS: ${CORE_LIBS}")
##
##