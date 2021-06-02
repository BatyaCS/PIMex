QT       += core gui
QT       += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ObserverNF.cpp \
    TaskWorkspace.cpp \
    main.cpp \
    MainWindow.cpp \
    programState.cpp \
    settingsWindow.cpp \
    tasks.cpp \
    tasksListSpace.cpp \
    workspace.cpp

HEADERS += \
    MainWindow.h \
    ObserverNF.h \
    TaskWorkspace.h \
    programState.h \
    settingsWindow.h \
    tasks.h \
    tasksListSpace.h \
    workspace.h

FORMS += \
    MainWindow.ui \
    settingsWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
