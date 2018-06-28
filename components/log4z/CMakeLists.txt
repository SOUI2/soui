
include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(log4z_header
	log4z.h
)

set(log4z_src
    log4z.cpp)

source_group("Header Files" FILES ${log4z_header})
source_group("Source Files" FILES ${log4z_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (log4z_src  ${log4z_src} log4z.rc)
    add_library(log4z SHARED ${log4z_src} ${log4z_header})
else()
    add_library(log4z STATIC ${log4z_src} ${log4z_header})
endif()

set(COM_LIBS ${COM_LIBS} log4z CACHE INTERNAL "com_lib")
set_target_properties (log4z PROPERTIES
    FOLDER components
)
cotire(log4z)
