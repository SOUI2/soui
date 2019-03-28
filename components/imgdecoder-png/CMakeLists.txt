#
# SOUI cmake配置文件
#
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

# imgdecoder-png

set(imgdecoder-png_header
	imgdecoder-png.h
	decoder-apng.h
)

set(imgdecoder-png_src
    decoder-apng.cpp
    imgdecoder-png.cpp
)

source_group("Header Files" FILES ${imgdecoder-png_header})
source_group("Source Files" FILES ${imgdecoder-png_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (imgdecoder-png_src  ${imgdecoder-png_src} imgdecoder-png.rc)
    add_library(imgdecoder-png SHARED ${imgdecoder-png_src} ${imgdecoder-png_header})
    target_link_libraries(imgdecoder-png png zlib)
else()
    add_library(imgdecoder-png STATIC ${imgdecoder-png_src} ${imgdecoder-png_header})
    target_compile_definitions(imgdecoder-png
      PUBLIC SOUI_IMAGE_DECODER_USE_LIBPNG_STATIC
    )
endif()

target_compile_definitions(imgdecoder-png
  PUBLIC SOUI_IMAGE_DECODER_USE_LIBPNG
)
target_include_directories(imgdecoder-png
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${PROJECT_SOURCE_DIR}/config
  PRIVATE ${PROJECT_SOURCE_DIR}/utilities/include
  PRIVATE ${PROJECT_SOURCE_DIR}/SOUI/include
  PRIVATE ${PROJECT_SOURCE_DIR}/third-part/png
  PRIVATE ${PROJECT_SOURCE_DIR}/third-part/zlib
)
target_link_libraries(imgdecoder-png png)

set(COM_LIBS ${COM_LIBS} imgdecoder-png CACHE INTERNAL "com_lib")
set_target_properties (imgdecoder-png PROPERTIES
    FOLDER components/imgdecoder
)
cotire(imgdecoder-png)
