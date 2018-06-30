TARGET = LKTracking
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Full_Program
TEMPLATE = app
CONFIG += console

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    io_communicate.cpp \
    pid.cpp \
    track_frame.cpp \
    log_location.cpp \
    onepolecontrol.cpp \
    predictivecontroller.cpp \
    video_recording.cpp \
    field_of_view/fieldofviewwindow.cpp \
    controls_window.cpp \
    selection_window.cpp \
    field_of_view/fov.cpp

HEADERS  += \
    cbw.h \
    io_communicate.h \
    pid.h \
    track_frame.h \
    log_location.h \
    onepolecontrol.h \
    predictivecontroller.h \
    video_recording.h \
    field_of_view/fieldofviewwindow.h \
    controls_window.h \
    selection_window.h \
    field_of_view/fov.h


FORMS    += \
    fieldofviewwindow.ui \
    controls_window.ui \
    selection_window.ui

# This is for the DAQ
unix|win32: LIBS += -static -L$$PWD -lcbw32 -lcbw64

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

# To obtain screenshots and stuff
LIBS += -lGdi32

# openCV
#INCLUDEPATH += C:\opencv\build\include

#LIBS += C:\opencv_build\bin\libopencv_core310.dll
#LIBS += C:\opencv_build\bin\libopencv_highgui310.dll
#LIBS += C:\opencv_build\bin\libopencv_imgproc310.dll
#LIBS += C:\opencv_build\bin\libopencv_videoio310.dll
#LIBS += C:\opencv_build\bin\libopencv_video310.dll


INCLUDEPATH += F:\opencv\opencv\build\include

LIBS += F:\opencv-build\bin\libopencv_core310.dll
LIBS += F:\opencv-build\bin\libopencv_highgui310.dll
LIBS += F:\opencv-build\bin\libopencv_imgcodecs310.dll
LIBS += F:\opencv-build\bin\libopencv_imgproc310.dll
LIBS += F:\opencv-build\bin\libopencv_features2d310.dll
LIBS += F:\opencv-build\bin\libopencv_calib3d310.dll


# Multitasking
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

