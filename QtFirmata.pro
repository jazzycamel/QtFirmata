QT       -= gui
QT       += serialport

TARGET = QtFirmata
TEMPLATE = lib

DEFINES += QTFIRMATA_LIBRARY

SOURCES += qtfirmata.cpp \

HEADERS += qtfirmata.h\
        qtfirmata_global.h \

unix {
    target.path = /usr/lib
    INSTALLS += target
}
