#include "views/ServerMainWindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>

#include <QtHttpServer/QHttpServer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 데이터 디렉토리 생성 확인
    QDir dataDir("./../../data/");
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
        qDebug() << "Created data directory:" << dataDir.absolutePath();
    }

    QDir logDir("./../../log/");
    if (!logDir.exists()) {
        logDir.mkpath(".");
        qDebug() << "Created log directory:" << logDir.absolutePath();
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "PlantServant_" + QLocale(locale).name();
        qDebug() << baseName << Qt::endl;
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    ServerMainWindow w;

    QList<QScreen*> screens = QGuiApplication::screens();
    int targetScreenIndex = 2;
    QRect screenGeometry = screens[targetScreenIndex]->geometry();

    // 해당 화면의 중앙에 윈도우 배치
    int x = screenGeometry.x() + (screenGeometry.width() - w.width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - w.height()) / 2;
    w.move(x,y);
    w.show();

    return a.exec();
}
