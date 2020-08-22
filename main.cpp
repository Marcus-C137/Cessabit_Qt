#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include "logger.h"
#include "firebase.h"
#include "arduinocoms.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(network);
Q_LOGGING_CATEGORY(network, "network", QtWarningMsg);


int main(int argc, char *argv[])
{
    logger::attach();

    ArduinoComs coms;
    Firebase firebase;
    firebase.login("marcushoutzager@gmail.com", "password");


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
