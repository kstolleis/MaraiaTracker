#-------------------------------------------------
#
# Project created by QtCreator 2013-03-10T11:30:51
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = MaraiaTracker_5_1
TEMPLATE = app

include(qextserialport.pri)

SOURCES +=  main.cpp\
            trackingwindow.cpp \
            unitconversions.cpp \
            gpspoint.cpp \
            seriallist.cpp

HEADERS  += trackingwindow.h \
            unitconversions.h \
            gpspoint.h \
            seriallist.h

FORMS    += trackingwindow.ui

RESOURCES += Images.qrc
