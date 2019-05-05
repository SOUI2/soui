#add_definitions(-D_CRT_SECURE_NO_WARNINGS) // stb_image.h:4577 don't define _CRT_SECURE_NO_WARNINGS (affects user code)

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
    target_compile_definitions(imgdecoder-stb
      PUBLIC SOUI_IMAGE_DECODER_USE_STB_STATIC
    )
endif()

target_compile_definitions(imgdecoder-stb
  PUBLIC SOUI_IMAGE_DECODER_USE_STB
)
target_include_directories(imgdecoder-stb
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${PROJECT_SOURCE_DIR}/config
  PRIVATE ${PROJECT_SOURCE_DIR}/utilities/include
  PRIVATE ${PROJECT_SOURCE_DIR}/SOUI/include
)

set(COM_LIBS ${COM_LIBS} imgdecoder-stb CACHE INTERNAL "com_lib")
set_target_properties (imgdecoder-stb PROPERTIES
    FOLDER components/imgdecoder
)
cotire(imgdecoder-stb)

SOUI_INSTALL_TARGETS(imgdecoder-stb)
