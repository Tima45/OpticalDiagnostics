#-------------------------------------------------
#
# Project created by QtCreator 2018-02-24T11:21:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = OpticalDiagnostics
TEMPLATE = app

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
        mainwindow.cpp \
    plot/qcustomplot.cpp \
    tracermanager.cpp \
    showpicform.cpp \
    calibrationform.cpp \
    clickablelabel.cpp \
    indicator.cpp \
    capturemanager.cpp

HEADERS  += mainwindow.h \
    plot/qcustomplot.h \
    tracermanager.h \
    showpicform.h \
    calibrationform.h \
    clickablelabel.h \
    indicator.h \
    capturemanager.h

FORMS    += mainwindow.ui \
    showpicform.ui \
    calibrationform.ui
RC_FILE = icon.rc

INCLUDEPATH+=C:/Librarys/OpenCV2/include
INCLUDEPATH+=C:/Librarys/OpenCV2/include/opencv

LIBS+=C:/Librarys/OpenCV2/dll/libopencv_core320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_calib3d320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_objdetect320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_photo320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_shape320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_stitching320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_superres320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_videostab320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_features2d320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_flann320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_highgui320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_imgcodecs320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_imgproc320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_ml320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_imgproc320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_imgcodecs320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_video320.dll
LIBS+=C:/Librarys/OpenCV2/dll/libopencv_videoio320.dll

LIBS+=C:/Librarys/OpenCV2/dll/opencv_ffmpeg320.dll



INCLUDEPATH += "C:/Librarys/WaveletSpectrum/include"
LIBS += "C:/Librarys/WaveletSpectrum/lib/libWaveletSpectrum.a"
LIBS += "C:/Librarys/gsl/lib/libgsl.a"

RESOURCES += \
    indicatorpicturesresource.qrc

DISTFILES += \
    indicatorOff.png \
    indicatorOn.png
