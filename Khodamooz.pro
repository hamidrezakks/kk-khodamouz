#-------------------------------------------------
#
# Project created by QtCreator 2014-09-08T10:26:23
#
#-------------------------------------------------

QT       += core gui\
            sql\
            multimedia
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Khodamooz
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    addlesson.cpp \
    addquestion.cpp \
    setting.cpp \
    about.cpp \
    printsetting.cpp

HEADERS  += mainwindow.h \
    addlesson.h \
    addquestion.h \
    setting.h \
    about.h \
    printsetting.h

FORMS    += mainwindow.ui \
    addlesson.ui \
    addquestion.ui \
    setting.ui \
    about.ui \
    printsetting.ui

OTHER_FILES +=
