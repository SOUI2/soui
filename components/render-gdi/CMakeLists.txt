add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

set(render-gdi_header
	stdafx.h
	GradientFillHelper.h
	render-gdi.h
)

set(render-gdi_src 
    GradientFillHelper.cpp
    render-gdi.cpp
)

source_group("Header Files" FILES ${render-gdi_header})
source_group("Source Files" FILES ${render-gdi_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (render-gdi_src  ${render-gdi_src} render-gdi.rc)
    add_library(render-gdi SHARED ${render-gdi_src} ${render-gdi_header})
    target_link_libraries(render-gdi utilities)
else()
    add_library(render-gdi STATIC ${render-gdi_src} ${render-gdi_header})
endif()

set(COM_LIBS ${COM_LIBS} render-gdi CACHE INTERNAL "com_lib")
set_target_properties (render-gdi PROPERTIES
    FOLDER components/render
)
cotire(render-gdi)
