include_directories(..)

SET (SMILEY_HEADERS
	${CMAKE_SOURCE_DIR}/controls.extend/smiley/SSmileyCtrl.h
)

SET (SMILEY_SRCS
	SSmileyCtrl.cpp
)

source_group("Header Files" FILES ${SMILEY_HEADERS})
source_group("Source Files" FILES ${SMILEY_SRCS})

ADD_LIBRARY (smiley STATIC ${SMILEY_SRCS} ${SMILEY_HEADERS})
set_target_properties (smiley PROPERTIES
    FOLDER third-part
)
cotire(smiley)
