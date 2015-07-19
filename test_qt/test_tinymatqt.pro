QT       += core
QT       -= gui

TARGET = test_tinymatqt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DESTDIR=./

DEFINES += TINYMAT_USES_QVARIANT



SOURCES += test_tinymat.cpp \
           ../tinymatwriter.cpp

QMAKE_CFLAGS += -Wall
QMAKE_CXXFLAGS += -Wall
