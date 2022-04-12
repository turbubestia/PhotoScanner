CONFIG -= debug release
CONFIG += debug_and_release

QT += core gui widgets

Debug: TARGET = pscand
Release: TARGET = pscan

TEMPLATE = app

SOURCES += src/lib/AbstractLayer.cpp \
           src/ImageSizeControl.cpp \
           src/lib/LayerGraphicWidget.cpp \
           src/lib/PixelLayer.cpp \
           src/lib/RoiEditorLayer.cpp \
           src/lib/Utils.cpp

HEADERS += src/lib/AbstractLayer.h \
           src/ImageSizeControl.h \
           src/lib/LayerGraphicWidget.h \
           src/lib/PixelLayer.h \
           src/lib/RoiEditorLayer.h \
           src/lib/Utils.h

SOURCES += src/main.cpp \
    src/MainWindow.cpp \
    src/ImageProcessor.cpp \
	
HEADERS += src/MainWindow.h \
    src/ImageProcessor.h

INCLUDEPATH += src/lib
INCLUDEPATH += $$(MINGW_HOME)/include/opencv4
INCLUDEPATH += $$(MINGW_HOME)/include/opencv4/opencv2

LIBS += -L$$(MINGW_HOME)/lib
LIBS += -llibopencv_core -llibopencv_highgui -llibopencv_imgcodecs
LIBS += -llibopencv_imgproc -llibopencv_features2d -llibopencv_calib3d -llibopencv_photo
LIBS += -llibexiv2

RC_ICONS = media/icon.ico

RESOURCES += qdarkstyle/style.qrc

FORMS += \
    src/ImageSizeControl.ui
