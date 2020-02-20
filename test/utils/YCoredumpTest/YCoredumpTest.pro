QT += core
QT -= gui

CONFIG += c++11
QMAKE_CXXFLAGS += -rdynamic


TARGET = YCoredumpTest
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
