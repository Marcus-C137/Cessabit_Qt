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
    #ifdef QT_DEBUG
     qputenv("BUILD_TYPE", QByteArray("DEBUG"));
    #else
     qputenv("BUILD_TYPE", QByteArray("RELEASE"));
    #endif

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    ChartData cd;
    Logger::attach();
    WifiManager wifiManager;
    ArduinoComs coms;
    Singleton<Localdb>::GetInstance();
    Singleton<Localdb>::GetInstance().checkDBs();
    Singleton<Localdb>::GetInstance().checkUserAccount();
    Singleton<Localdb>::GetInstance().loadAlarmTemps();
    Singleton<Localdb>::GetInstance().loadPortsOn();
    Singleton<Localdb>::GetInstance().loadGains();
    Singleton<Localdb>::GetInstance().loadTemps();
    QQmlContext* context = engine.rootContext();
    Firebase firebase;
    firebase.loginViaDB();
    firebase.setWifiManager(&wifiManager);
    firebase.setArduinoComs(&coms);
    context->setContextProperty("arduinoComms", &coms);
    context->setContextProperty("chartDataObj", &cd);
    context->setContextProperty("wifiManager", &wifiManager);
    context->setContextProperty("firebase", &firebase);
    engine.load(url);
    return app.exec();
}
