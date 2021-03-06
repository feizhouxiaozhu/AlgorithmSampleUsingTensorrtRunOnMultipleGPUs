





QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = AlgorithmDemo

DEFINES += __STDC_CONSTANT_MACROS

QMAKE_CXXFLAGS += -fpermissive

TEMPLATE = app


CUDA_PATH=/opt/cuda10.1
CUDNN_PATH=/opt/cuda10.1/cudnn7.6.2
FFMPEG_PATH=/opt/ffmpeg3.4.6
#DLIB_PATH=/opt/dlib_cuda10.1

INCLUDEPATH += \
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


SOURCES += \
    $$files(*.cpp) \
    $$files(utils/*.cpp) \
    $$files(videoio/*.cpp)


HEADERS += \
    $$files(utils/*.h) \
    $$files(videoio/*.h)

