# 
# SOUI cmake配置文件
#
add_subdirectory(imgdecoder-gdip)
add_subdirectory(imgdecoder-png)
add_subdirectory(imgdecoder-stb)
add_subdirectory(imgdecoder-wic)


add_subdirectory(render-gdi)
add_subdirectory(render-skia)
add_subdirectory(resprovider-zip)

add_subdirectory(translator)
add_subdirectory(ScriptModule-Lua)

add_subdirectory(log4z)
add_subdirectory(resprovider-7zip)
add_subdirectory(TaskLoop)

#include(CheckCXXCompilerFlag)
#CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#if(COMPILER_SUPPORTS_CXX11)
#if(MSVC_VERSION LESS 1700)
#	message(STATUS "build resprovider-7zip OFF")
#else()
#	add_subdirectory(resprovider-7zip)
#	message(STATUS "build resprovider-7zip ON")
#endif()
