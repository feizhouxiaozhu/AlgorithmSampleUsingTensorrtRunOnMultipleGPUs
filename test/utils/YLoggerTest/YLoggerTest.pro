





QT += core
QT -= gui

CONFIG += c++11
QMAKE_CXXFLAGS += -fpermissive

TARGET = YLoggerTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SRC_PATH=$$PWD/../../../src/utils/

HEADERS += \
    $$files($$SRC_PATH/*.h) \


SOURCES += \
    main.cpp \
    $$files($$SRC_PATH/*.cpp) \


LIBS += \
#system
    -lglog
