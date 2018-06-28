# 
# SOUI cmake配置文件
#

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(include)

set(RCS_MAIN soui-sys-resource.rc)
set(HDRS_MAIN resource.h)

source_group("Header Files" FILES ${HDRS_MAIN})
source_group("Source Files" FILES ${SRCS_MAIN})
source_group("Resource Files" FILES ${RCS_MAIN})

add_library (soui-sys-resource SHARED ${SRCS_MAIN} ${HDRS_MAIN} ${RCS_MAIN})
set_target_properties(soui-sys-resource PROPERTIES LINK_FLAGS "/NOENTRY")

# add_library(soui-sys-resource SHARED soui-sys-resource.rc)
cotire(soui-sys-resource)
