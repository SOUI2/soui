add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NON_CONFORMING_SWPRINTFS)

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/utilities/include)
include_directories(${PROJECT_SOURCE_DIR}/SOUI/include)
include_directories(${PROJECT_SOURCE_DIR}/third-part/lua-52/src)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(ScriptModule-lua_header
	src/stdafx.h
#	src/require.h
	src/ScriptModule-Lua.h
	src/exports/exp_app.h
	src/exports/exp_Basic.h
	src/exports/exp_eventArgs.h
	src/exports/exp_hostwnd.h
	src/exports/exp_msgbox.h
	src/exports/exp_pugixml.h
	src/exports/exp_ResProvider.h
	src/exports/exp_ScriptModule.h
	src/exports/exp_strcpcvt.h
	src/exports/exp_string.h
	src/exports/exp_Window.h
	src/exports/exp_Object.h
	src/exports/exp_ctrls.h
	lua_tinker/lua_tinker.h
)

set(ScriptModule-lua_src
    src/ScriptModule-Lua.cpp
    src/exports/exp_soui.cpp
    lua_tinker/lua_tinker.cpp
)

source_group("Header Files" FILES ${ScriptModule-lua_header})
source_group("Source Files" FILES ${ScriptModule-lua_src})

if (NOT ENABLE_SOUI_COM_LIB)
    set (ScriptModule-Lua_src  ${ScriptModule-lua_src} ScriptModule-lua.rc)
    add_library(ScriptModule-Lua SHARED ${ScriptModule-lua_src} ${ScriptModule-lua_header})
    target_link_libraries(ScriptModule-Lua lua utilities soui)
    #cotire(ScriptModule-Lua)
else()
    add_library(ScriptModule-Lua STATIC ${ScriptModule-lua_src} ${ScriptModule-lua_header})
    #cotire(ScriptModule-Lua)
endif()

if (NOT ENABLE_SOUI_CORE_LIB)
	message(STATUS "[DLL_CORE] ScriptModule-Lua: ON")
    set(COM_LIBS ${COM_LIBS} ScriptModule-Lua CACHE INTERNAL "com_lib")
else()
	message(STATUS "[LIB_CORE] ScriptModule-Lua: OFF")
endif()

set_target_properties (ScriptModule-Lua PROPERTIES
    FOLDER components
)
cotire(ScriptModule-Lua)
