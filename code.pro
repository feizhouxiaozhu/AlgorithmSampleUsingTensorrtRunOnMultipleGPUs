

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = AlgorithmDemo


QMAKE_CXXFLAGS += -fpermissive

TEMPLATE = app

LIBS += \
#system
    -lglog

SOURCES += \
    $$files(*.cpp) \
    $$files(utils/*.cpp) \
    $$files(algorithm/*.cpp) \


HEADERS += \
    $$files(utils/*.h) \
    $$files(algorithm/*.h) \


