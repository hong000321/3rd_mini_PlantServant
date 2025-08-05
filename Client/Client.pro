QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controllers/ClientSocket.cpp \
    main.cpp \
    models/entity/ChatRoom.cpp \
    models/entity/ChatUnit.cpp \
    models/entity/Plant.cpp \
    models/entity/Post.cpp \
    models/entity/User.cpp \
    models/service/ChatService.cpp \
    models/service/UserService.cpp \
    views/ClientMainWindow.cpp \
    views/JoinMemberMainWindow.cpp \
    views/LoginMainWindow.cpp

HEADERS += \
    controllers/ClientSocket.h \
    models/entity/ChatRoom.h \
    models/entity/ChatUnit.h \
    models/entity/Plant.h \
    models/entity/Post.h \
    models/entity/User.h \
    models/service/ChatService.h \
    models/service/UserService.h \
    views/ClientMainWindow.h \
    views/JoinMemberMainWindow.h \
    views/LoginMainWindow.h \
    utils/CustomErrorCodes.h


INCLUDEPATH += \
    views \
    models \
    controllers

FORMS += \
    views/ClientMainWindow.ui \
    views/JoinMemberMainWindow.ui \
    views/LoginMainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
