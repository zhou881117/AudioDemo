QT       += core gui

QT  +=  multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DESTDIR = $$PWD/../bin
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioCache.cpp \
    Audio_Player.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AudioCache.h \
    Audio_Player.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix|win32: LIBS += -L$$PWD/../3rdparty/portaudio/lib/ -lportaudio_x86

INCLUDEPATH += $$PWD/../3rdparty/portaudio/include
DEPENDPATH += $$PWD/../3rdparty/portaudio/include

win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lavcodec
win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lavdevice
win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lavfilter
win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lavformat
win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lavutil
#win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lpostproc
win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lswresample
win32: LIBS += -L$$PWD/../3rdparty/ffmpeg3.4/lib/ -lswscale

INCLUDEPATH += $$PWD/../3rdparty/ffmpeg3.4/include
DEPENDPATH += $$PWD/../3rdparty/ffmpeg3.4/include
