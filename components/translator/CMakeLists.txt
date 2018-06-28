# 
# SOUI cmake配置文件
#

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

set(translator_header
	stdafx.h
	translator.h
	targetver.h
)

set(translator_src 
    dllmain.cpp
    translator.cpp)

source_group("Header Files" FILES ${translator_header})
source_group("Source Files" FILES ${translator_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (translator_src  ${translator_src} translator.rc)
    add_library(translator SHARED ${translator_src} ${translator_header})
    target_link_libraries(translator utilities soui)
else()
    add_library(translator STATIC ${translator_src} ${translator_header})
endif()

set(COM_LIBS ${COM_LIBS} translator CACHE INTERNAL "com_lib")
set_target_properties (translator PROPERTIES
    FOLDER components
)
cotire(translator)
