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

Q_DECLARE_LOGGING_CATEGORY(network);
Q_LOGGING_CATEGORY(network, "network", QtWarningMsg);

QSqlDatabase Localdb::sql_db = QSqlDatabase::addDatabase("QSQLITE");
Localdb Localdb::localdb;

int main(int argc, char *argv[])
{

    Localdb::sql_db.setDatabaseName("/home/pi/cessabit.db");
    if(!Localdb::sql_db.open()){
        qInfo() << "could not open db";
    }else{
        qInfo() << "opened database";
    }

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
    Firebase firebase;
    firebase.login("marcushoutzager@gmail.com", "password");

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("chartDataObj", &cd);
    return app.exec();
}
