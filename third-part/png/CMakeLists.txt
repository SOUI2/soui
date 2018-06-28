# CMakeLists.txt

# Copyright (C) 2007-2014 Glenn Randers-Pehrson

# This code is released under the libpng license.
# For conditions of distribution and use, see the disclaimer
# and license in png.h

cmake_minimum_required(VERSION 2.4.4)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

set(PNGLIB_MAJOR 1)
set(PNGLIB_MINOR 5)
set(PNGLIB_RELEASE 19)
set(PNGLIB_NAME libpng${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_RELEASE})

# SET LIBNAME
set(PNG_LIB_NAME png${PNGLIB_MAJOR}${PNGLIB_MINOR})


include_directories(../zlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DPNG_CONFIGURE_LIBPNG)
add_definitions(-D_CRT_SECURE_NO_DEPRECATE)




# OUR SOURCES
set(libpng_headers
	png.h
	pngconf.h
	pngdebug.h
	pnginfo.h
	pnglibconf.h
	pngpriv.h
	pngstruct.h
)

set(libpng_sources
	png.c
	pngerror.c
	pngget.c
	pngmem.c
	pngpread.c
	pngread.c
	pngrio.c
	pngrtran.c
	pngrutil.c
	pngset.c
	pngtrans.c
	pngwio.c
	pngwrite.c
	pngwtran.c
	pngwutil.c
)

source_group("Header Files" FILES ${libpng_headers})
source_group("Source Files" FILES ${libpng_sources})

add_library(png STATIC ${libpng_sources} ${libpng_headers})
set(COM_LIBS ${COM_LIBS} png CACHE INTERNAL "com_lib")
set_target_properties (png PROPERTIES
    FOLDER third-part
)
cotire(png)
