CONFIG -= qt
CONFIG += stl_off  

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
#多核编译
QMAKE_CXXFLAGS += /MP


DEPENDPATH += .
INCLUDEPATH += . \
			   $(SOUIPATH)/utilities/include \
			   $(SOUIPATH)/soui/include \
			   $(SOUIPATH)/components \
			   $(SOUIPATH)/config

LIBPATH += $(SOUIPATH)/bin