#
# 1394020320@qq.com
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lexlib)

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRCS *.cxx)
file(GLOB_RECURSE CURRENT_RC *.rc *.rc2)

source_group("Header Files" FILES ${CURRENT_HEADERS})
source_group("Source Files" FILES ${CURRENT_SRCS})
source_group("Resource Files" FILES ${CURRENT_RC})

add_library(Scintilla STATIC ${CURRENT_HEADERS} ${CURRENT_SRCS} ${CURRENT_RC})

#set(COM_LIBS ${COM_LIBS} Scintilla CACHE INTERNAL "com_lib")
set_target_properties (Scintilla PROPERTIES
    FOLDER third-part
)
