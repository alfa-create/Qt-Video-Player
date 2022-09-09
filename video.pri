SOURCES += \
    $$PWD/VideoDefs.cpp \
    $$PWD/decoder.cpp \
    $$PWD/framequeue.cpp \
    $$PWD/packetqueue.cpp \
    $$PWD/videocontroller.cpp \
    $$PWD/videohandlers.cpp

HEADERS += \
    $$PWD/VideoDefs.h \
    $$PWD/VideoState.h \
    $$PWD/decoder.h \
    $$PWD/framequeue.h \
    $$PWD/packetqueue.h \
    $$PWD/videocontroller.h \
    $$PWD/videohandlers.h

INCLUDEPATH += $${_PRO_FILE_PWD_}/3rdparty/SDL/include
DEPENDPATH  += $${_PRO_FILE_PWD_}/3rdparty/SDL/include
win32:LIBS += -L$${_PRO_FILE_PWD_}/3rdparty/SDL/bin -lSDL2
unix:LIBS += -lSDL2

win32:INCLUDEPATH += $${_PRO_FILE_PWD_}/3rdparty/FFMPEG/win32/include
win32:DEPENDPATH +=  $${_PRO_FILE_PWD_}/3rdparty/FFMPEG/win32/include
unix:INCLUDEPATH += $${_PRO_FILE_PWD_}/3rdparty/FFMPEG/linux/include
unix:DEPENDPATH +=  $${_PRO_FILE_PWD_}/3rdparty/FFMPEG/linux/include

win32:LIBS += -lm -lz

win32:LIBS +=  -L$${_PRO_FILE_PWD_}/3rdparty/FFMPEG/win32/bin -lavutil-57 -lavdevice-59 -lavfilter-8 -lavformat-59 -lswresample-4 -lswscale-6 -lavcodec-59 -lpostproc-56
unix:LIBS += -L$${_PRO_FILE_PWD_}/3rdparty/FFMPEG/linux/lib -lavutil-56 -lavdevice-58 -lavfilter-7 -lavformat-58 -lswresample-3 -lswscale-5 -lavcodec-58

