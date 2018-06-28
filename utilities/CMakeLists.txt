# 
# SOUI cmake配置文件
#

add_definitions(-DUTILITIES_EXPORTS -D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)

file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRCS  *.cpp)


source_group("Header Files" FILES ${CURRENT_HEADERS})
source_group("Source Files" FILES ${CURRENT_SRCS})

if (NOT ENABLE_SOUI_CORE_LIB)
    set (source  ${CURRENT_SRCS} utilities.rc)
    add_library(utilities SHARED ${CURRENT_SRCS} ${CURRENT_HEADERS})
    target_link_libraries(utilities png zlib)
else()
    add_library(utilities STATIC ${CURRENT_SRCS} ${CURRENT_HEADERS})
endif()

# add_library(utilities SHARED ${CURRENT_SRCS} ${CURRENT_HEADERS})

set_target_properties(utilities PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")

set(CORE_LIBS ${CORE_LIBS} utilities CACHE INTERNAL "core_lib")