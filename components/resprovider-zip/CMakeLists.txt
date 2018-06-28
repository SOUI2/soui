add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(${PROJECT_SOURCE_DIR}/third-part/zlib)
include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)

set(resprovider-zip_header
	stdafx.h
	SResProviderZip.h
	ZipArchive.h
)

set(resprovider-zip_src 
    cursoricon.cpp
    SResProviderZip.cpp
    zipArchive.cpp
)

source_group("Header Files" FILES ${resprovider-zip_header})
source_group("Source Files" FILES ${resprovider-zip_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (resprovider-zip_src  ${resprovider-zip_src} resprovider-zip.rc)
    add_library(resprovider-zip SHARED ${resprovider-zip_src} ${resprovider-zip_header})
    target_link_libraries(resprovider-zip utilities zlib)
else()
    add_library(resprovider-zip STATIC ${resprovider-zip_src} ${resprovider-zip_header})
endif()

set(COM_LIBS ${COM_LIBS} resprovider-zip CACHE INTERNAL "com_lib")
set_target_properties (resprovider-zip PROPERTIES
    FOLDER components
)
cotire(resprovider-zip)
