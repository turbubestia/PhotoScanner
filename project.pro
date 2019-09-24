CONFIG -= debug release
CONFIG += debug_and_release

QT += core gui widgets

Debug: TARGET = pscand
Release: TARGET = pscan

TEMPLATE = app

SOURCES += src/main.cpp \
	src/MainWindow.cpp
	
HEADERS += src/MainWindow.h

INCLUDEPATH += $$(MINGW_HOME)/include/opencv

LIBS += -L$$(MINGW_HOME)/lib
LIBS += -llibopencv_core -llibopencv_highgui -llibopencv_imgcodecs
LIBS += -llibopencv_imgproc -llibopencv_features2d -llibopencv_calib3d