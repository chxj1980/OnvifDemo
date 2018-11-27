# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = app
TARGET = 7-OnvifDetectDevice
DESTDIR = ./Debug
QT += core gui
CONFIG += debug
DEFINES += QT_LARGEFILE_SUPPORT QT_DLL WITH_OPENSSL

INCLUDEPATH += ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug \
    ./../6-OnvifDemo \
    ./../OpenSSL-Win32/include \
    ./../ffmpeg-20181118-529debc-win32/include
win32:INCLUDEPATH += ./../ffmpeg-20181118-529debc-win32/include/Win

win32:LIBS += -L"./../OpenSSL-Win32/lib/VC" \
    -L"./../ffmpeg-20181118-529debc-win32/lib" \
    -llibssl32MDd \
    -llibcrypto32MDd \
    -lavcodec \
    -lavformat \
    -lavutil \
    -lswscale
unix:LIBS  += -L"/usr/lib/x86_64-linux-gnu" \
    -Wl,-rpath=./:../bin:./Debug \
    -lssl \
    -lcrypto \
    -lavcodec-ffmpeg \
    -lavformat-ffmpeg \
    -lavutil-ffmpeg \
    -lswscale-ffmpeg

DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
HEADERS += ./onvifdetectdevice.h \
    ./VideoPlayerThread.h \
    ../6-OnvifDemo/dom.h \
    ../6-OnvifDemo/mecevp.h \
    ../6-OnvifDemo/smdevp.h \
    ../6-OnvifDemo/soapH.h \
    ../6-OnvifDemo/soapStub.h \
    ../6-OnvifDemo/stdsoap2.h \
    ../6-OnvifDemo/threads.h \
    ../6-OnvifDemo/wsaapi.h \
    ../6-OnvifDemo/wsseapi.h
SOURCES += ./main.cpp \
    ./onvifdetectdevice.cpp \
    ./VideoPlayerThread.cpp \
    ../6-OnvifDemo/dom.cpp \
    ../6-OnvifDemo/mecevp.c \
    ../6-OnvifDemo/smdevp.c \
    ../6-OnvifDemo/soapC.cpp \
    ../6-OnvifDemo/soapClient.cpp \
    ../6-OnvifDemo/stdsoap2.cpp \
    ../6-OnvifDemo/threads.c \
    ../6-OnvifDemo/wsaapi.cpp \
    ../6-OnvifDemo/wsseapi.cpp
FORMS += ./onvifdetectdevice.ui
RESOURCES += ./onvifdetectdevice.qrc
