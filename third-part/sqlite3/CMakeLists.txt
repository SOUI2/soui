#
# 1394020320@qq.com
#
add_definitions(
	-D_CRT_SECURE_NO_WARNINGS
	-DSQLITE_ENABLE_COLUMN_METADATA
	-DSQLITE_HAS_CODEC=1
	-DCODEC_TYPE=CODEC_TYPE_AES128
	-DSQLITE_CORE
	-DTHREADSAFE
	-DSQLITE_SECURE_DELETE
	-DSQLITE_SOUNDEX
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SQLITE_SRC
	sqlite3secure.c
)

source_group("Source Files" FILES ${SQLITE_SRC})

add_library(sqlite3 STATIC ${SQLITE_HEADER} ${SQLITE_SRC})

#set(COM_LIBS ${COM_LIBS} sqlite3 CACHE INTERNAL "com_lib")
set_target_properties (sqlite3 PROPERTIES
    FOLDER third-part
)
cotire(sqlite3)
