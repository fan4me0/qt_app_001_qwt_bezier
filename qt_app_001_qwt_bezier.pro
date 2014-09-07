#-------------------------------------------------
#
# Project created by QtCreator 2013-06-10T20:48:44
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = qt_app_001_qwt_bezier
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sliders.cpp \
    userData.cpp

HEADERS  += mainwindow.h \
    sliders.h \
    userData.h

#------------------------------------------------------------
# Qwt library settings

QWT_PATH = /home/franto/builds/libs/qwt/qwt_610_bezier
QWT_PATH_INC = $${QWT_PATH}/src
QWT_PATH_LIB = $${QWT_PATH}/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../builds/libs/qwt/qwt_620_trunk140414/lib/release/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../builds/libs/qwt/qwt_620_trunk140414/lib/debug/ -lqwt
else:unix: LIBS += -L$${QWT_PATH_LIB} -lqwt

INCLUDEPATH += $${QWT_PATH_INC}
DEPENDPATH += $${QWT_PATH_INC}

#------------------------------------------------------------
# use this library build for application deplyement build as library is built against
# standard Qt5 library install in system folders (libraries installed via apt-get)
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../builds/qwt_610_bezier/lib/release/ -lqwt
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../builds/qwt_610_bezier/lib/debug/ -lqwt
#else:unix: LIBS += -L/home/franto/builds/qwt_610_bezier_qt_def/lib/ -lqwt

#INCLUDEPATH += /home/franto/builds/qwt_610_bezier_qt_def/src
#DEPENDPATH += /home/franto/builds/qwt_610_bezier_qt_def/src

OTHER_FILES += \
    LICENSE.md \
    README.md \
    .gitignore
