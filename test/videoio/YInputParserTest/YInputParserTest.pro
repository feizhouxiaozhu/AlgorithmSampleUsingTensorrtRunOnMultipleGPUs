





QT += gui core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += __STDC_CONSTANT_MACROS

QMAKE_CXXFLAGS += -fpermissive

TARGET = YInputParserTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SRC_PATH=$$PWD/../../../src

SOURCES += \
    $$files(*.cpp) \
    $$files($$SRC_PATH/utils/*.cpp) \
    $$files($$SRC_PATH/videoio/*.cpp) \
    yglwidget.cpp

HEADERS += \
    $$files(*.h) \
    $$files($$SRC_PATH/utils/*.h) \
    $$files($$SRC_PATH/videoio/*.h) \
    yglwidget.h

CUDA_PATH=/opt/cuda10.1
CUDNN_PATH=/opt/cuda10.1/cudnn7.6.2
FFMPEG_PATH=/opt/ffmpeg3.4.6
#DLIB_PATH=/opt/dlib_cuda10.1

INCLUDEPATH += \
    $$SRC_PATH \
#cuda
    $$CUDA_PATH/include \
#ffmpeg
    $$FFMPEG_PATH/include \


LIBS += \
#cuda
    -L$$CUDA_PATH/lib64 \
    -Wl,-rpath,$$CUDA_PATH/lib64 \
    -lcuda -lcudart \
#cudnn
    -L$$CUDNN_PATH/lib64 \
    -Wl,-rpath,$$CUDNN_PATH/lib64 \
    -lcudnn \
#ffmpeg
    -L$$FFMPEG_PATH/lib \
    -Wl,-rpath,$$FFMPEG_PATH/lib \
    -lavformat -lavcodec -lavutil -lswscale -lswresample \
#system
    -lglog
