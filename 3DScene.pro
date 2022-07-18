#-------------------------------------------------
#
# Project created by QtCreator 2018-09-07T09:02:52
#
#-------------------------------------------------

QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3DScene
TEMPLATE = app
RESOURCES += resources/style.qrc
INCLUDEPATH = / \
    src

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
        src/main.cpp \
        src/mainwindow.cpp \
    src/clickablelabel.cpp \
    src/texturedialog.cpp \
    src/projection.cpp \
    src/vertex.cpp \
    src/mesh.cpp \
    src/face.cpp \
    src/camera.cpp \
    src/color.cpp \
    src/texture.cpp \
    src/xmlparser.cpp \
    src/scene.cpp \
    src/light.cpp \
    src/animationdialog.cpp \
    src/animation.cpp

HEADERS += \
        src/mainwindow.h \
    src/clickablelabel.h \
    src/texturedialog.h \
    src/projection.h \
    src/vertex.h \
    src/mesh.h \
    src/face.h \
    src/camera.h \
    src/color.h \
    src/texture.h \
    src/xmlparser.h \
    src/scene.h \
    src/light.h \
    src/animationdialog.h \
    src/animation.h

FORMS += \
        src/ui/mainwindow.ui \
    src/ui/texturedialog.ui \
    src/ui/animationdialog.ui
