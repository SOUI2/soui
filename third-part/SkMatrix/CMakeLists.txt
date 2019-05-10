#
# 1394020320@qq.com
#

add_definitions(-D_CRT_SECURE_NO_WARNINGS -DSK_BUILD_FOR_WIN32)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SkMatrix_header
    include/SkCamera.h
    include/SkFloatBits.h
    include/SkMatrix.h
    include/SkPoint.h
    include/SkPreConfig.h
    include/SkRect.h
    include/SkScalar.h
    include/SkSize.h
    include/SkTLazy.h
    include/SkTypes.h
)

set(SkMatrix_src
    src/SkCamera.cpp
    src/SkMatrix.cpp
    src/SkPoint.cpp
    src/SkRect.cpp
)

source_group("Header Files" FILES ${SkMatrix_header})
source_group("Source Files" FILES ${SkMatrix_src})

if (NOT ENABLE_SOUI_COM_LIB)
    add_definitions(-DSKMATRIX_DLL -DBUILD_MATRIX)
    add_library(SkMatrix SHARED ${SkMatrix_src} ${SkMatrix_header})
else()
    add_library(SkMatrix STATIC ${SkMatrix_src} ${SkMatrix_header})
endif()

target_include_directories(SkMatrix
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${PROJECT_SOURCE_DIR}/config
  PRIVATE ${PROJECT_SOURCE_DIR}/utilities/include
  PRIVATE ${PROJECT_SOURCE_DIR}/SOUI/include
)

set(COM_LIBS ${COM_LIBS} SkMatrix CACHE INTERNAL "com_lib")
set_target_properties (SkMatrix PROPERTIES
    FOLDER third-part
)
cotire(SkMatrix)
