#
#
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}src)

set(headers
	src/lapi.h
	src/lauxlib.h
	src/lcode.h
	src/lctype.h
	src/ldebug.h
	src/ldo.h
	src/lfunc.h
	src/lgc.h
	src/llex.h
	src/llimits.h
	src/lmem.h
	src/lobject.h
	src/lopcodes.h
	src/lparser.h
	src/lstate.h
	src/lstring.h
	src/ltable.h
	src/ltm.h
	src/lua.h
	src/lua.hpp
	src/luaconf.h
	src/lualib.h
	src/lundump.h
	src/lvm.h
	src/lzio.h
)

set(sources
	src/lapi.c
	src/lauxlib.c
	src/lbaselib.c
	src/lbitlib.c
	src/lcode.c
	src/lcorolib.c
	src/lctype.c
	src/ldblib.c
	src/ldebug.c
	src/ldo.c
	src/ldump.c
	src/lfunc.c
	src/lgc.c
	src/linit.c
	src/liolib.c
	src/llex.c
	src/lmathlib.c
	src/lmem.c
	src/loadlib.c
	src/lobject.c
	src/lopcodes.c
	src/loslib.c
	src/lparser.c
	src/lstate.c
	src/lstring.c
	src/lstrlib.c
	src/ltable.c
	src/ltablib.c
	src/ltm.c
	src/lundump.c
	src/lvm.c
	src/lzio.c
)

source_group("Header Files" FILES ${headers})
source_group("Source Files" FILES ${sources})

add_library(lua STATIC ${sources} ${headers})
set_target_properties(lua PROPERTIES OUTPUT_NAME lua-52)
# set_target_properties(lua PROPERTIES PREFIX lib)
set_target_properties (lua PROPERTIES
    FOLDER third-part
)
