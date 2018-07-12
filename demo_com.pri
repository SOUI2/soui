CONFIG -= qt
CONFIG += stl_off  
CONFIG += debug_and_release
!CONFIG(DISABLE_WCHAR){
	#打开wchar_t为内置类型
	QMAKE_CXXFLAGS += /Zc:wchar_t
}

!CONFIG(MBCS){
	CharacterSet = 1
}
else{
	CharacterSet = 2
	DEFINES -= UNICODE
}

CONFIG(USING_MT){
	#使用MT链接CRT
	QMAKE_CXXFLAGS_RELEASE += /MT
	QMAKE_CXXFLAGS_DEBUG += /MTd
}

CONFIG(debug, debug|release) {
	OBJECTS_DIR =   debug
	DESTDIR = debug
}
else {
	OBJECTS_DIR =   release
	DESTDIR = release
}

CONFIG(x64){
	OBJECTS_DIR = $$OBJECTS_DIR"64"
	DESTDIR = $$DESTDIR"64"
}

CONFIG(x64){
	QMAKE_LFLAGS += /MACHINE:X64
	DEFINES -= WIN32
	DEFINES += _WIN64
             LIBPATH += $(SOUIPATH)/bin64
}
else{
	QMAKE_LFLAGS += /MACHINE:X86
             LIBPATH += $(SOUIPATH)/bin
}
#多核编译
QMAKE_CXXFLAGS += /MP
QMAKE_CXXFLAGS += -Fd$(IntDir)

DEPENDPATH += .
INCLUDEPATH += . \
			   $(SOUIPATH)/utilities/include \
			   $(SOUIPATH)/soui/include \
			   $(SOUIPATH)/components \
			   $(SOUIPATH)/config

