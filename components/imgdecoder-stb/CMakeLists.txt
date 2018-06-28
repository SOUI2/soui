#add_definitions(-D_CRT_SECURE_NO_WARNINGS) // stb_image.h:4577 don't define _CRT_SECURE_NO_WARNINGS (affects user code)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

set(imgdecoder-stb_header
	imgdecoder-stb.h
	stb_image.h
)

set(imgdecoder-stb_src
    imgdecoder-stb.cpp
)

source_group("Header Files" FILES ${imgdecoder-stb_header})
source_group("Source Files" FILES ${imgdecoder-stb_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (imgdecoder-stb_src  ${imgdecoder-stb_src} imgdecoder-stb.rc)
    add_library(imgdecoder-stb SHARED ${imgdecoder-stb_src} ${imgdecoder-stb_header})
else()
    add_library(imgdecoder-stb STATIC ${imgdecoder-stb_src} ${imgdecoder-stb_header})
endif()

set(COM_LIBS ${COM_LIBS} imgdecoder-stb CACHE INTERNAL "com_lib")
set_target_properties (imgdecoder-stb PROPERTIES
    FOLDER components/imgdecoder
)
cotire(imgdecoder-stb)
