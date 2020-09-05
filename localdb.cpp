#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include <QString>
#include <QDateTime>
#include <QVector>
#include "localdb.h"




Localdb::Localdb(QObject *parent) : QObject(parent)
{
    if(sql_db.isOpen()){
        qInfo() << "is open";
    }else{
        qInfo() << "not open";
    }
}

Localdb::~Localdb()
{

}

Localdb& Localdb::Get()
{
    return localdb;
}



void Localdb::addTempReading(int port, float temp)
{
    // Time zone map
    // Eastern - America/New_York 5h
    // Central - America/Kentucky/Monticello 6h
    // Mountain - America/Mexico_City 7h
    // Pacific - America/Los_Angeles 8h
    QSqlQuery query(sql_db);
    QDateTime time(QDateTime::currentDateTime());
    qint64 time_sec = time.toSecsSinceEpoch();
    //qInfo() << "LOCALDB: current time, secs since epoch = " << time.toString();
    QString command = "INSERT INTO port" + QString::number(port) + " VALUES " + "(" + QString::number(time_sec) + "," + QString::number(temp) + ");";
    //qInfo() << "LOCALDB: command = " << command;
    query.prepare(command);
    if (!query.exec())
        qWarning() << "ERROR: " << query.lastError().text();
    //qInfo() << "LOCALDB: addTempReading(): added";
    emit newTempReading(port, time, temp);


}

QVector<dbVal> Localdb::getTemps(int port, int count){

    QVector<dbVal> timeTempsVec;
    QSqlQuery query;
    query.prepare("SELECT * FROM port" + QString::number(port) + " ORDER BY time DESC LIMIT " + QString::number(count));
    if (!query.exec())
        qWarning() << "ERROR: " << query.lastError().text();
    if(query.first()){
        qInfo() << "QUERY FIRST RESULTS" << query.value(0).toString();
    }
    else{
      qInfo() << "NO QUERY FIRST";
    }

    for (int i = 0; i < count; i++){
        qInfo() << "LOCALDB: QUERY RESULT time: " + QString::number(i) + "  =  " << query.value(0).toString();
        qInfo() << "LOCALDB: QUERY RESULT temp: " + QString::number(i) + "  =  " << query.value(1).toString();
        int time  = query.value(0).toInt();
        float temp = query.value(1).toFloat();
        timeTempsVec.push_back({time, temp});
        query.next();

    }
    return timeTempsVec;

}

bool Localdb::testPlugin()
{
    qInfo() << "Testing Plugin";
    QPluginLoader loader("/usr/local/qt5pi/plugins/sqldrivers/libqsqlite.so");
    qInfo() << loader.metaData();

    if(loader.load())
    {
        qInfo() << "Loaded the plugin";
        return true;
    }

    qInfo() << loader.errorString();
    return false;
}



