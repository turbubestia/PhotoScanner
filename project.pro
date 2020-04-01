CONFIG -= debug release
CONFIG += debug_and_release

QT += core gui widgets

Debug: TARGET = pscand
Release: TARGET = pscan

TEMPLATE = app

SOURCES += src/main.cpp \
	src/MainWindow.cpp \
	src/PageViewer.cpp \
	src/PageMapper.cpp
	
HEADERS += src/MainWindow.h \
	src/PageViewer.h \
	src/PageMapper.h

INCLUDEPATH += $$(MINGW_HOME)/include/opencv4
INCLUDEPATH += $$(MINGW_HOME)/include/opencv4/opencv2

LIBS += -L$$(MINGW_HOME)/lib
LIBS += -llibopencv_core -llibopencv_highgui -llibopencv_imgcodecs
LIBS += -llibopencv_imgproc -llibopencv_features2d -llibopencv_calib3d -llibopencv_photo

RC_ICONS = media/icon.ico

RESOURCES += qdarkstyle/style.qrc