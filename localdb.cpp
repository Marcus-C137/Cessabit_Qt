#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include <QString>
#include <QDateTime>
#include <QVector>
#include <QTimer>
#include "localdb.h"




Localdb::Localdb(QObject *parent) : QObject(parent)
{
    sql_db = QSqlDatabase::addDatabase("QSQLITE");
    sql_db.setDatabaseName("/home/pi/cessabit.db");
    if(sql_db.open()){
        qInfo() << "could not open db";
    }else{
        qInfo() << "opened database";
    }
    if(sql_db.isOpen()){
        qInfo() << "is open";
    }else{
        qInfo() << "not open";
    }

    cleanDBbyMin = new QTimer(this);

//    cleanDBbyHour = new QTimer(this);
//    cleanDBbyDay = new QTimer(this);
//    cleanDBbyMonth = new QTimer(this);
    //cleanDBbyMin->moveToThread(&DB_workerThread);
    //DB_workerThread.start();
    //connect(&DB_workerThread, SIGNAL(started()), this, SLOT(startTimer()));
    connect(cleanDBbyMin,SIGNAL(timeout()) , this, SLOT(cleanDB_Min()));
//    connect(cleanDBbyHour, SIGNAL(timeout()), this, SLOT(cleanDB_Hour()));
//    connect(cleanDBbyDay, SIGNAL(timeout()), this, SLOT(cleanDB_Day()));
//    connect(cleanDBbyMonth, SIGNAL(timeout()), this, SLOT(cleanDB_Month()));
    cleanDBbyMin->start(120000);
//    cleanDBbyHour->start(2*60*60*1000);
//    cleanDBbyDay->start(2*24*60*60*1000);
//    cleanDBbyMonth->start(2*24*60*60*1000+2000);
}

Localdb::~Localdb()
{

}

void Localdb::startTimer(){
    cleanDBbyMin->start(10000);
}

void Localdb::checkDBs()
{
    QSqlQuery query(sql_db);
    QString commandPre = "create table if not exists ";
    QStringList tables = {"port1_min", "port1_hour", "port1_day", "port1_month",
                          "port2_min", "port2_hour", "port2_day", "port2_month",
                          "port3_min", "port3_hour", "port3_day", "port3_month",
                          "port4_min", "port4_hour", "port4_day", "port4_month"};
    QString params = "(time DATETIME, temp REAL)";
    for (int i=0; i< tables.length(); i++){
        QString command = commandPre + tables[i] + params;
        if (!query.exec(command)){
            qWarning() << "LOCALDB: ERROR: could not create table " << tables[i];
        }
    }

}


void Localdb::addTempReading(int port, float temp)
{
    // Time zone map
    // Eastern - America/New_York 5h
    // Central - America/Chicago 6h
    // Mountain - America/Mexico_City 7h
    // Pacific - America/Los_Angeles 8h

    // ADD TO DB
    QSqlQuery query;
    QDateTime time(QDateTime::currentDateTime());
    qint64 time_sec = time.toSecsSinceEpoch();
    //qInfo() << "LOCALDB: current time, secs since epoch = " << time.toString();
    QString command = "INSERT INTO port" + QString::number(port) + "_min " + "VALUES " + "(" + QString::number(time_sec) + "," + QString::number(temp) + ");";
    //qInfo() << "LOCALDB: command = " << command;
    query.prepare(command);
    if (!query.exec()) qWarning() << "ERROR: " << query.lastError().text();
    //qInfo() << "LOCALDB: addTempReading(): added";

    emit newTempReading(port, time, temp);
}


QVector<dbVal> Localdb::getTemps(int port, int count){

    QVector<dbVal> timeTempsVec;
    QSqlQuery query;
    query.prepare("SELECT * FROM (SELECT * FROM port" + QString::number(port) + " ORDER BY time DESC LIMIT " + QString::number(count) + ") ORDER BY time ASC");
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


void Localdb::cleanDB_Min()
{
    //CHECK IF MIN IS TOO LARGE
    //qInfo() << Q_FUNC_INFO;
    for (int i=1; i<5 ; i++){
        QString port = "port" + QString::number(i) + "_min";
        QSqlQuery query_min;
        QString command_min = "SELECT COUNT(temp) FROM " + port;
        query_min.prepare(command_min);
        if(!query_min.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from port1_min";
            qWarning() << query_min.lastError().text();
        }

        query_min.first();
        //qInfo() << "LOCALDB: - size of " + port + " db before delete " << query_min.value(0).toString();

        //DELETE
        int dbSize = query_min.value(0).toInt();
        query_min.finish();
        //qInfo() << "LOCALDB: cleanDB_min() - dbSize: " << QString::number(dbSize);

        if (dbSize > 6){
            QSqlQuery query_del;
            int delLimit = dbSize - 20;
            QString command_del = "DELETE FROM port1_min LIMIT 10;"; //+ QString::number(delLimit) + ";";
            query_del.prepare("DELETE FROM " + port + " WHERE time IN (SELECT time FROM " + port + " LIMIT " + QString::number(delLimit) + ")" );
            if(!query_del.exec()) {
                qWarning() << "LOCALDB: - could not delete from database";
                qWarning() << query_del.lastError().text();
            }
        }

        //Query after
        QSqlQuery query_min2;
        QString command_min2 = "SELECT COUNT(temp) FROM " + port;
        query_min2.prepare(command_min2);
        if(!query_min2.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from port1_min";
            qWarning() << query_min2.lastError().text();
        }

        query_min2.first();
        //qInfo() << "LOCALDB: - size of " + port + " db after delete " << query_min2.value(0).toString();


    }
}

void Localdb::cleanDB_Hour()
{
    //CHECK IF HOUR IS TOO LARGE
    for (int i=1; i<5 ; i++){
        QString port = "port" + QString::number(i) + "_hour";
        QSqlQuery query_min;
        QString command_min = "SELECT COUNT(temp) FROM " + port;
        query_min.prepare(command_min);
        if(!query_min.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from " + port;
            qWarning() << query_min.lastError().text();
        }

        query_min.first();
        qInfo() << "LOCALDB: - size of " + port + " db before delete " << query_min.value(0).toString();

        //DELETE
        int dbSize = query_min.value(0).toInt();
        QSqlQuery query_del;
        if (dbSize > 6 && port == 1){
            int delLimit = dbSize - 6;
            QString command_del = "DELETE FROM port1_min LIMIT 10;"; //+ QString::number(delLimit) + ";";
            query_del.prepare("DELETE FROM " + port + " WHERE time IN (SELECT time FROM " + port + "LIMIT " + QString::number(delLimit) + ")" );
            if(!query_del.exec()) {
                qWarning() << "LOCALDB: - could not delete from database";
                qWarning() << query_del.lastError().text();
            }
        }

        //Query after
        QSqlQuery query_min2;
        QString command_min2 = "SELECT COUNT(temp) FROM " + port;
        query_min2.prepare(command_min2);
        if(!query_min2.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from " + port;
            qWarning() << query_min2.lastError().text();
        }

        query_min2.first();
        qInfo() << "LOCALDB: - size of " + port + " db after delete " << query_min2.value(0).toString();

    }
}

void Localdb::cleanDB_Day()
{
    //CHECK IF DAY IS TOO LARGE
    for (int i=1; i<5 ; i++){
        QString port = "port" + QString::number(i) + "_day";
        QSqlQuery query_min;
        QString command_min = "SELECT COUNT(temp) FROM " + port;
        query_min.prepare(command_min);
        if(!query_min.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from " + port;
            qWarning() << query_min.lastError().text();
        }

        query_min.first();
        qInfo() << "LOCALDB: - size of " + port + " db before delete " << query_min.value(0).toString();

        //DELETE
        int dbSize = query_min.value(0).toInt();
        QSqlQuery query_del;
        if (dbSize > 6 && port == 1){
            int delLimit = dbSize - 6;
            QString command_del = "DELETE FROM port1_min LIMIT 10;"; //+ QString::number(delLimit) + ";";
            query_del.prepare("DELETE FROM " + port + " WHERE time IN (SELECT time FROM " + port + "LIMIT " + QString::number(delLimit) + ")" );
            if(!query_del.exec()) {
                qWarning() << "LOCALDB: - could not delete from database";
                qWarning() << query_del.lastError().text();
            }
        }

        //Query after
        QSqlQuery query_min2;
        QString command_min2 = "SELECT COUNT(temp) FROM " + port;
        query_min2.prepare(command_min2);
        if(!query_min2.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from " + port;
            qWarning() << query_min2.lastError().text();
        }

        query_min2.first();
        qInfo() << "LOCALDB: - size of " + port + " db after delete " << query_min2.value(0).toString();


    }
}

void Localdb::cleanDB_Month()
{
    //CHECK IF MONTH IS TOO LARGE
    for (int i=1; i<5 ; i++){
        QString port = "port" + QString::number(i) + "_month";
        QSqlQuery query_min;
        QString command_min = "SELECT COUNT(temp) FROM " + port;
        query_min.prepare(command_min);
        if(!query_min.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from " + port;
            qWarning() << query_min.lastError().text();
        }

        query_min.first();
        qInfo() << "LOCALDB: - size of " + port + " db before delete " << query_min.value(0).toString();

        //DELETE
        int dbSize = query_min.value(0).toInt();
        QSqlQuery query_del;
        if (dbSize > 6 && port == 1){
            int delLimit = dbSize - 6;
            QString command_del = "DELETE FROM port1_min LIMIT 10;"; //+ QString::number(delLimit) + ";";
            query_del.prepare("DELETE FROM " + port + " WHERE time IN (SELECT time FROM " + port + "LIMIT " + QString::number(delLimit) + ")" );
            if(!query_del.exec()) {
                qWarning() << "LOCALDB: - could not delete from database";
                qWarning() << query_del.lastError().text();
            }
        }

        //Query after
        QSqlQuery query_min2;
        QString command_min2 = "SELECT COUNT(temp) FROM " + port;
        query_min2.prepare(command_min2);
        if(!query_min2.exec()) {
            qWarning() << "LOCALDB: addTempReading() - ERROR: could not get count from " + port;
            qWarning() << query_min2.lastError().text();
        }

        query_min2.first();
        qInfo() << "LOCALDB: - size of " + port + " db after delete " << query_min2.value(0).toString();


    }

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



