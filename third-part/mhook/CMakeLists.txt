SET (MHOOK_HEADERS
	disasm-lib/cpu.h
	disasm-lib/disasm.h
	disasm-lib/disasm_x86.h
	disasm-lib/disasm_x86_tables.h
	disasm-lib/misc.h
)

SET (MHOOK_SRCS
	disasm-lib/cpu.c
	disasm-lib/disasm.c
	disasm-lib/disasm_x86.c
	disasm-lib/misc.c
	mhook-lib/mhook.cpp
)

source_group("Header Files" FILES ${MHOOK_HEADERS})
source_group("Source Files" FILES ${MHOOK_SRCS})

ADD_LIBRARY (mhook STATIC ${MHOOK_SRCS} ${MHOOK_HEADERS})
set_target_properties (mhook PROPERTIES
    FOLDER third-part
)
cotire(mhook)
