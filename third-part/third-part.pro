TEMPLATE = subdirs
TARGET = third-part
CONFIG(x64){
TARGET = $$TARGET"64"
}
DEPENDPATH += .
INCLUDEPATH += .

include(../cpy-cfg.pri)

SUBDIRS += gtest
SUBDIRS += png
SUBDIRS += skia
SUBDIRS += zlib
SUBDIRS += lua-52
SUBDIRS += smiley
SUBDIRS += mhook
SUBDIRS += 7z
SUBDIRS += scintilla
SUBDIRS += sqlite3
SUBDIRS += jsoncpp
SUBDIRS += skmatrix

CONFIG(c++11){
#7z需要c11支持
	
}
