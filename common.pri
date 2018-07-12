CONFIG -= qt
CONFIG += stl_off  
INCLUDEPATH += $$dir/config

!CONFIG(MBCS){
	CharacterSet = 1
}
else{
	CharacterSet = 2
	DEFINES -= UNICODE
}

CONFIG(debug, debug|release) {
	OBJECTS_DIR =   $$dir/obj/debug/$$TARGET
	DESTDIR = $$dir/bin
	CONFIG(x64){
		DESTDIR = $$DESTDIR"64"
	}	
	QMAKE_LIBDIR += $$DESTDIR
}
else {
	OBJECTS_DIR =   $$dir/obj/release/$$TARGET
	DESTDIR = $$dir/bin
	CONFIG(x64){
		DESTDIR = $$DESTDIR"64"
	}
	QMAKE_LIBDIR += $$DESTDIR
}

#<--下面这段代码为debug和release生成不同的文件名
defineReplace(souiLibraryTarget) {
  unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1  
      !debug_and_release|build_pass {
	LIBRARY_NAME~= s,64,,
	RET = $$LIBRARY_NAME
      }
 
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

SAVE_TEMPLATE = $$TEMPLATE
TEMPLATE = fakelib
TARGET = $$qtLibraryTarget($$souiLibraryTarget($$TARGET))
TEMPLATE = $$SAVE_TEMPLATE
#-->

DEFINES += _CRT_SECURE_NO_WARNINGS
#DEFINES += _WIN32_WINNT=0x0502 #会导致vs2013编译skia失败，还是删掉吧。

#多核编译
QMAKE_CXXFLAGS += /MP

CONFIG(x64){
	QMAKE_LFLAGS += /MACHINE:X64
	DEFINES -= WIN32
	DEFINES += _WIN64
}
else{
	QMAKE_LFLAGS += /MACHINE:X86
}

!CONFIG(USING_CLR){
	#关闭RTTI
	QMAKE_CXXFLAGS_RTTI_ON += /GR-
}
else{
	QMAKE_CXXFLAGS += /clr
	CONFIG += exceptions_off
	#关闭异常
	QMAKE_CXXFLAGS -= -EHsc
}

QMAKE_CXXFLAGS += -Fd$(IntDir)

!CONFIG(DISABLE_WCHAR){
	#打开wchar_t为内置类型
	QMAKE_CXXFLAGS += /Zc:wchar_t
}

QMAKE_CXXFLAGS_RELEASE += /O1
QMAKE_CXXFLAGS_RELEASE += /Zi

CONFIG(CAN_DEBUG){
	#Release版本允许生产调试符号
	QMAKE_LFLAGS_RELEASE += /DEBUG
	QMAKE_LFLAGS_RELEASE += /OPT:REF /OPT:ICF
}

CONFIG(USING_MT){
	#使用MT链接CRT
	QMAKE_CXXFLAGS_RELEASE += /MT
	QMAKE_CXXFLAGS_DEBUG += /MTd
}

CONFIG(USING_CLR){
	#使用MD链接CRT
	QMAKE_CXXFLAGS_RELEASE -= /MT
	QMAKE_CXXFLAGS_DEBUG -= /MTd

	QMAKE_CXXFLAGS_RELEASE += /MD
	QMAKE_CXXFLAGS_DEBUG += /MDd
}

CONFIG(TOOLSET_XP){
	QMAKE_TARGET_OS=xp
}
QMAKE_CXXFLAGS += /wd4100 /wd4101 /wd4102 /wd4189 /wd4996
