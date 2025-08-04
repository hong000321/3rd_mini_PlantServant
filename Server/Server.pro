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
    src/controllers/FileTransferServer.cpp \
    src/controllers/FileTransfer.cpp \
    src/controllers/ProtocolController.cpp \
    src/controllers/SocketServer.cpp \
    src/models/entity/Entity.cpp \
    src/models/repository/ChatJsonRepo.cpp \
    src/models/repository/ChatRoomJsonRepo.cpp \
    src/models/repository/OrderItemJsonRepo.cpp \
    src/models/repository/OrderJsonRepo.cpp \
    src/models/repository/ProductJsonRepo.cpp \
    src/models/service/ChatManageService.cpp \
    src/models/service/LoginService.cpp \
    src/models/service/OrderManageService.cpp \
    src/models/service/ProductManageService.cpp \
    src/models/service/UserManageService.cpp \
    src/views/EnrollAdminWindow.cpp \
    src/views/ServerMainWindow.cpp \
    src/models/entity/chatlog/ChatRoom.cpp \
    src/models/entity/chatlog/ChatUnit.cpp \
    src/models/entity/order/Order.cpp \
    src/models/entity/order/OrderItem.cpp \
    src/models/entity/product/Product.cpp \
    src/models/entity/user/User.cpp \
    src/models/repository/UserJsonRepo.cpp \
    src/views/TestWindow.cpp


HEADERS += \
    src/ServerConfig.h \
    src/controllers/FileTransferServer.h \
    src/controllers/FileTransfer.h \
    src/controllers/HttpRestServer.h \
    src/controllers/ProtocolController.h \
    src/controllers/SocketServer.h \
    src/models/entity/Entity.h \
    src/models/repository/ChatJsonRepo.h \
    src/models/repository/ChatRoomJsonRepo.h \
    src/models/repository/OrderItemJsonRepo.h \
    src/models/repository/OrderJsonRepo.h \
    src/models/repository/ProductJsonRepo.h \
    src/models/service/ChatManageService.h \
    src/models/service/LoginService.h \
    src/models/service/OrderManageService.h \
    src/models/service/ProductManageService.h \
    src/models/service/UserManageService.h \
    src/views/EnrollAdminWindow.h \
    src/views/ServerMainWindow.h \
    src/models/entity/chatlog/ChatRoom.h \
    src/models/entity/chatlog/ChatUnit.h \
    src/models/entity/order/Order.h \
    src/models/entity/order/OrderItem.h \
    src/models/entity/product/Product.h \
    src/models/entity/user/User.h \
    src/models/repository/IRepository.h \
    src/models/repository/JsonRepo.h \
    src/models/repository/UserJsonRepo.h \
    src/utils/CustomErrorCodes.h \
    src/views/TestWindow.h

INCLUDEPATH += \
    $${SRC_DIR}

FORMS += \
    src/views/test_window.ui \
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
