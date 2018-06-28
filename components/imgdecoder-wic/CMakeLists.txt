include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

set(imgdecoder-wic_header
	stdafx.h
	imgdecoder-wic.h
	targetver.h
)

set(imgdecoder-wic_src
    imgdecoder-wic.cpp)

source_group("Header Files" FILES ${imgdecoder-wic_header})
source_group("Source Files" FILES ${imgdecoder-wic_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (imgdecoder-wic_src  ${imgdecoder-wic_src} imgdecoder-wic.rc)
    add_library(imgdecoder-wic SHARED ${imgdecoder-wic_src} ${imgdecoder-wic_header})
else()
    add_library(imgdecoder-wic STATIC ${imgdecoder-wic_src} ${imgdecoder-wic_header})
endif()

set(COM_LIBS ${COM_LIBS} imgdecoder-wic CACHE INTERNAL "com_lib")
set_target_properties (imgdecoder-wic PROPERTIES
    FOLDER components/imgdecoder
)
cotire(imgdecoder-wic)
