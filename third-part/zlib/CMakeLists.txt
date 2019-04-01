#INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

#-----------------------------------------------------------------------------
# Define zlib Library
#-----------------------------------------------------------------------------

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set (ZLIB_HEADERS
	crc32.h
	deflate.h
	inffast.h
	inffixed.h
	inflate.h
	inftrees.h
	trees.h
	zconf.h
	zconf.in.h
	zlib.h
	zutil.h
)

SET (ZLIB_SRCS
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzio.c
    infback.c
    inffast.c
    inflate.c
    inftrees.c
    trees.c
    uncompr.c
    zutil.c
)

source_group("Header Files" FILES ${ZLIB_HEADERS})
source_group("Source Files" FILES ${ZLIB_SRCS})

ADD_LIBRARY (zlib STATIC ${ZLIB_SRCS} ${ZLIB_HEADERS})
set(COM_LIBS ${COM_LIBS} zlib CACHE INTERNAL "com_lib")
set_target_properties (zlib PROPERTIES
    FOLDER third-part
)
cotire(zlib)

SOUI_INSTALL_TARGETS(zlib)
