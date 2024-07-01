#-------------------------------------------------
#
# Project created by QtCreator 2017-11-10T21:35:17
#
#-------------------------------------------------

QT += core gui
QT += serialport network
QT += bluetooth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

winrt: QT -= serialport

#android: QT += androidextras


QMAKE_CFLAGS = -Wno-unused-parameter

TARGET = bs2mgr
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        windowmain.cpp \
    cio.cpp \
    portwindow.cpp \
    device.cpp \
    dterminal.cpp \
    dlinereader.cpp \
    deviceworker.cpp \
    iodev.cpp \
    btwindow.cpp \
    helpwindow.cpp \
    widgetterminal.cpp \
    androidfiledialog.cpp

HEADERS += \
        windowmain.h \
    cio.h \
    portwindow.h \
    device.h \
    dterminal.h \
    dlinereader.h \
    deviceworker.h \
    iodev.h \
    btwindow.h \
    helpwindow.h \
    widgetterminal.h \
    annotation.h \
    androidfiledialog.h

FORMS += \
        windowmain.ui \
    portwindow.ui \
    widgetdevicetab.ui \
    btwindow.ui \
    helpwindow.ui \
    widgetterminal.ui

CONFIG += mobility
MOBILITY =

# icon
win32: RC_FILE = bs2mgr.rc

# Developer mode
#DEFINES += DEVELMODE

RESOURCES += \
    resources.qrc

DISTFILES += \
    howto.html \
    bs2mgr.rc \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

