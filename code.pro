

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = AlgorithmDemo


QMAKE_CXXFLAGS += -fpermissive

TEMPLATE = app


CUDA_PATH=/opt/cuda10.1
#DLIB_PATH=/opt/dlib_cuda10.1

INCLUDEPATH += \
#cuda
    $$CUDA_PATH/include \


LIBS += \
#cuda
    -L$$CUDA_PATH/lib64 \
    -Wl,-rpath,$$CUDA_PATH/lib64 \
    -lcuda -lcudart -lcudnn \
#system
    -lglog


SOURCES += \
    $$files(*.cpp) \
    $$files(test/*.cpp) \
    $$files(utils/*.cpp) \
    $$files(algorithm/*.cpp) \


HEADERS += \
    $$files(test/*.h) \
    $$files(utils/*.h) \
    $$files(algorithm/*.h) \
