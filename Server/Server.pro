QT       += core gui network httpserver

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# for root dir sources
TR_DIR = translation
SRC_DIR = src

# for .ui format files
CUI_DIR = ui

# ============================================= for MVC source
MODEL_DIR = $${SRC_DIR}/models
VIEWS_DIR = $${SRC_DIR}/views
CONTR_DIR = $${SRC_DIR}/controllers
UTILS_DIR = $${SRC_DIR}/utils

SOURCES += \
    src/controllers/HttpRestServer.cpp \
    src/main.cpp \
    src/ServerConfig.cpp \
    src/controllers/ProtocolController.cpp \
    src/controllers/SocketServer.cpp \
    src/models/entity/Entity.cpp \
    src/models/entity/Plant.cpp \
    src/models/entity/Post.cpp \
    src/models/repository/ChatJsonRepo.cpp \
    src/models/repository/ChatRoomJsonRepo.cpp \
    src/models/repository/PlantJsonRepo.cpp \
    src/models/repository/PostJsonRepo.cpp \
    src/models/service/ChatManageService.cpp \
    src/models/service/LoginService.cpp \
    src/models/service/PlantManageService.cpp \
    src/models/service/PostManageService.cpp \
    src/models/service/UserManageService.cpp \
    src/views/EnrollAdminWindow.cpp \
    src/views/ServerMainWindow.cpp \
    src/models/entity/ChatRoom.cpp \
    src/models/entity/ChatUnit.cpp \
    src/models/entity/User.cpp \
    src/models/repository/UserJsonRepo.cpp


HEADERS += \
    src/ServerConfig.h \
    src/controllers/HttpRestServer.h \
    src/controllers/ProtocolController.h \
    src/controllers/SocketServer.h \
    src/models/entity/Entity.h \
    src/models/entity/Plant.h \
    src/models/entity/Post.h \
    src/models/repository/ChatJsonRepo.h \
    src/models/repository/ChatRoomJsonRepo.h \
    src/models/repository/PlantJsonRepo.h \
    src/models/repository/PostJsonRepo.h \
    src/models/service/ChatManageService.h \
    src/models/service/LoginService.h \
    src/models/service/PlantManageService.h \
    src/models/service/PostManageService.h \
    src/models/service/UserManageService.h \
    src/views/EnrollAdminWindow.h \
    src/views/ServerMainWindow.h \
    src/models/entity/ChatRoom.h \
    src/models/entity/ChatUnit.h \
    src/models/entity/User.h \
    src/models/repository/IRepository.h \
    src/models/repository/JsonRepo.h \
    src/models/repository/UserJsonRepo.h \
    src/utils/CustomErrorCodes.h

INCLUDEPATH += \
    $${SRC_DIR}

FORMS += \
    src/views/enroll_admin_window.ui \
    src/views/server_main_window.ui

TRANSLATIONS += \
    translation/PlantServant_Server_en_US.ts \
    translation/PlantServant_Server_ko_KR.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    src/controllers/.gitkeep \
    src/models/repository/.gitkeep \
    src/models/service/.gitkeep

RESOURCES += \
    rmattack_data.qrc
