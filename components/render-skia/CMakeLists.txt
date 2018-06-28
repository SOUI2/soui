include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

include_directories(${PROJECT_SOURCE_DIR}/third-part/skia)
include_directories(${PROJECT_SOURCE_DIR}/third-part/skia/include)
include_directories(${PROJECT_SOURCE_DIR}/third-part/skia/include/config)
include_directories(${PROJECT_SOURCE_DIR}/third-part/skia/include/core)

set(render-skia_header
	stdafx.h
	drawtext-skia.h
	render-skia.h
	render-skia2-i.h
	render-skia2.h
	skia2rop2.h
	patheffect-skia.h
	pathmeasure-skia.h
)
set(render-skia_src 
	drawtext-skia.cpp
	render-skia.cpp
	render-skia2.cpp
	skia2rop2.cpp
	patheffect-skia.cpp
	pathmeasure-skia.cpp
)

source_group("Header Files" FILES ${render-skia_header})
source_group("Source Files" FILES ${render-skia_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (render-skia_src  ${render-skia_src} render-skia.rc)
    add_library(render-skia SHARED ${render-skia_src} ${render-skia_header})
    target_link_libraries(render-skia Usp10 opengl32 skia utilities)
else()
    add_library(render-skia STATIC ${render-skia_src} ${render-skia_header})
endif()

set(COM_LIBS ${COM_LIBS} render-skia CACHE INTERNAL "com_lib")
set_target_properties (render-skia PROPERTIES
    FOLDER components/render
)
cotire(render-skia)
