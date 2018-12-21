TEMPLATE = subdirs
TARGET = components
CONFIG(x64){
TARGET = $$TARGET"64"
}
DEPENDPATH += .
INCLUDEPATH += .

include(../cpy-cfg.pri)

SUBDIRS += imgdecoder-wic
SUBDIRS += render-gdi
SUBDIRS += render-skia
SUBDIRS += translator
SUBDIRS += resprovider-zip
SUBDIRS += imgdecoder-stb
SUBDIRS += imgdecoder-png
SUBDIRS += imgdecoder-gdip
SUBDIRS += ScriptModule-LUA
SUBDIRS += log4z
SUBDIRS += resprovider-7zip
SUBDIRS += TaskLoop

imgdecoder-png.depends += zlib png
render-skia.depends += skia
resprovider-zip.depends += zlib utilities
translator.depends += utilities
resprovider-7zip.depends += 7z utilities
ScriptModule-LUA.depends += soui lua-52
TaskLoop.depends += utilities
