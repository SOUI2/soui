# 
# SOUI cmake配置文件
# 

#
# 1394020320@qq.com
#

add_definitions(-DSOUI_EXPORTS -D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRCS  *.cpp)
file(GLOB_RECURSE CURRENT_SOUIS  *.xml *.idx *.ico)
file(GLOB_RECURSE CURRENT_RC *.rc *.rc2)

source_group("Header Files" FILES ${CURRENT_HEADERS})
source_group("Source Files" FILES ${CURRENT_SRCS})
source_group("SoUI Resouece" FILES ${CURRENT_SOUIS})
source_group("Resource Files" FILES ${CURRENT_RC})

set(source ${CURRENT_HEADERS} ${CURRENT_SRCS} ${CURRENT_SOUIS})

if (NOT ENABLE_SOUI_CORE_LIB)
    set (source  ${source} soui.rc)
    add_library(soui SHARED ${source})
    target_link_libraries(soui utilities)
else()
    add_library(soui STATIC ${source})
endif()
set_target_properties(soui PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "include/souistd.h")
cotire(soui)

set(CORE_LIBS ${CORE_LIBS} soui CACHE INTERNAL "core_lib")