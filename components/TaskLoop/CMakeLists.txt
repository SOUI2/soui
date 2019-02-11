# 
# SOUI cmake配置文件
#

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

set(TaskLoop_header
	semaphore.h
	TaskLoop.h
	thread.h
)

set(TaskLoop_src 
	semaphore.cpp
	TaskLoop.cpp
	thread.cpp
)

source_group("Header Files" FILES ${TaskLoop_header})
source_group("Source Files" FILES ${TaskLoop_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (TaskLoop_src  ${TaskLoop_src} TaskLoop.rc)
    add_library(TaskLoop SHARED ${TaskLoop_src} ${TaskLoop_header})
    target_link_libraries(TaskLoop utilities)
else()
    add_library(TaskLoop STATIC ${TaskLoop_src} ${TaskLoop_header})
endif()

set(COM_LIBS ${COM_LIBS} TaskLoop CACHE INTERNAL "com_lib")
set_target_properties (TaskLoop PROPERTIES
    FOLDER components
)
cotire(TaskLoop)
