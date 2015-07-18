QT       += core
QT       -= gui

TARGET = untitled
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += TINYMAT_USES_QVARIANT


SOURCES += test_tinymat.cpp \
           ../tinymatwriter.cpp
