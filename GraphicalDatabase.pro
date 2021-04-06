QT       += core gui sql printsupport xml

#Application version
VERSION_MAJOR = 0
VERSION_MINOR = 2
VERSION_BUILD = 0 #$$system(version.bat)
VERSION_BUILD_MINOR = 2

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"\
       "VERSION_BUILD_MINOR=$$VERSION_BUILD_MINOR"


 DEFINES += QCUSTOMPLOT_USE_OPENGL
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DatabaseAccessModule/databaseaccess.cpp \
    DatabaseAccessModule/trackinfo.cpp \
    PlotModule/plot.cpp \
    PlotModule/qcustomplot/qcustomplot.cpp \
    RideModule/ataperegistrationchecker.cpp \
    RideModule/ride.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp

HEADERS += \
    DatabaseAccessModule/databaseaccess.h \
    DatabaseAccessModule/trackinfo.h \
    PlotModule/plot.h \
    PlotModule/qcustomplot/qcustomplot.h \
    RideModule/ataperegistrationchecker.h \
    RideModule/ride.h \
    mainwindow.h \
    settings.h
LIBS += -lglu32 -lopengl32
FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
