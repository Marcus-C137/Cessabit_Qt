#include <QApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QLoggingCategory>
#include <QQmlContext>
#include <QString>
#include "logger.h"
#include "firebase.h"
#include "arduinocoms.h"
#include "localdb.h"
#include "chartdata.h"
#include "singleton.h"
#include "wifimanager.h"

Q_DECLARE_LOGGING_CATEGORY(network);
Q_LOGGING_CATEGORY(network, "network", QtWarningMsg);



int main(int argc, char *argv[])
{

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    ChartData cd;
    Logger::attach();
    ArduinoComs coms;
    WifiManager wifiManager;
    Singleton<Localdb>::GetInstance();
    Singleton<Localdb>::GetInstance().checkDBs();
    Singleton<Localdb>::GetInstance().cleanDBbyMin->start(10000);
    Firebase firebase;
    firebase.login("marcushoutzager@gmail.com", "password");

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("arduinoComms", &coms);
    context->setContextProperty("chartDataObj", &cd);
    context->setContextProperty("wifiManager", &wifiManager);
    return app.exec();
}
