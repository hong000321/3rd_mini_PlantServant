QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controller/ClientSocket.cpp \
    main.cpp \
    model/Chat.cpp \
    model/Plant.cpp \
    model/Post.cpp \
    model/User.cpp \
    view/ClientMainWindow.cpp

HEADERS += \
    controller/ClientSocket.h \
    model/Chat.h \
    model/Plant.h \
    model/Post.h \
    model/User.h \
    view/ClientMainWindow.h

INCLUDEPATH += \
    view \
    model \
    controller

FORMS += \
    view/ClientMainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
