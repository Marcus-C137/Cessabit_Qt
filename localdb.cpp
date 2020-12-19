#include <QtMath>
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

QStringList tempLabel = {"Set", "High", "Low"};
QStringList portLabel = {"1", "2", "3", "4"};


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
    dbCounter = 0;
    setAlms.resize(4);
    setAlms[0].resize(4);
    setAlms[1].resize(4);
    setAlms[2].resize(4);
    setAlms[3].resize(4);

}

Localdb::~Localdb()
{

}

void Localdb::startTimer(){
    cleanDBbyMin->start(10000);
}

void Localdb::loadAlarmTemps()
{
    for(int i=0; i<4; i++){
        getAlarmTemps(i+1);
    }
}

void Localdb::checkDBs()
{
    QSqlQuery query(sql_db);
    QString commandPre = "create table if not exists ";
    QStringList tables = {"port1_min", "port1_hour", "port1_day", "port1_month",
                          "port2_min", "port2_hour", "port2_day", "port2_month",
                          "port3_min", "port3_hour", "port3_day", "port3_month",
                          "port4_min", "port4_hour", "port4_day", "port4_month",
                         };
    QString params = "(time DATETIME, temp REAL, power REAL)";

    for (int i=0; i< tables.length(); i++){
        QString command = commandPre + tables[i] + params;
        if (!query.exec(command)){
            qWarning() << "LOCALDB: ERROR: could not create table " << tables[i];
        }
    }

    params = "(uname TEXT, password TEXT)";
    if(!query.exec(commandPre + "userAccount" + params)){
        qWarning() <<  Q_FUNC_INFO << "Could not make table userAccount";
    }

    params = "(uname TEXT, password TEXT)";
    if(!query.exec(commandPre + "userAccount" + params)){
        qWarning() <<  Q_FUNC_INFO << "Could not make table userAccount";
    }

    params = "(port INT, setAlarm TEXT, val REAL)";
    if(!query.exec(commandPre + "setAlarmTemps" + params)){
        qWarning() << Q_FUNC_INFO << "Could not make table setAlarmTemps";
    }
    //Fill set alarm table
    commandPre= "INSERT INTO setAlarmTemps(port,setAlarm) ";
    for (int i=0; i < 12; i++){
        QString command = commandPre + "SELECT " + portLabel[qFloor(i/3)] +",\"" + tempLabel[i%3] + "\" "
                                      "WHERE NOT EXISTS("
                                      "SELECT 1 "
                                      "FROM setAlarmTemps "
                                      "WHERE port = " + portLabel[qFloor((i/3))] + " AND setAlarm=\""+ tempLabel[i%3] + "\");" ;

        if(!query.exec(command)){
            qWarning() << Q_FUNC_INFO << "Could not fill table setAlarmTemps";
        };
    }
}

QStringList Localdb::checkUserAccount()
{
    QSqlQuery query;
    QStringList accountInfo = {"", ""};
    if(!query.exec("SELECT * FROM userAccount")){
        qWarning() << Q_FUNC_INFO << query.lastError().text();
        return accountInfo;
    }
    if(!query.first()){
        qInfo() << Q_FUNC_INFO <<  "NO QUERY FIRST";
        return accountInfo;
    }
    qInfo() << "QUERY FIRST RESULTS" << query.value(0).toString();
    accountInfo[0] = query.value(0).toString();
    accountInfo[1] = query.value(1).toString();
    return accountInfo;
}

void Localdb::addUserInfo(QString uname, QString password)
{
    QSqlQuery query;
    QString command = "REPLACE INTO userAccount(rowid, uname, password) VALUES(1,\"" + uname + "\", \"" +password + "\");" ;
    query.prepare(command);
    if (!query.exec()) qWarning() << "ERROR: " << query.lastError().text();
    qInfo() << "LOCALDB: addUserInfo()" << uname << " " << password;
}

void Localdb::changeAlarmTemp(int port, int alarmLabel, qreal val)
{
    QSqlQuery query;
    QString command= "UPDATE setAlarmTemps SET val=" + QString::number(val) + " WHERE port=" + QString::number(port) + " AND setAlarm=\"" + tempLabel[alarmLabel] + "\";" ;
    setAlms[port-1][alarmLabel] = val;
    qInfo() << command;
    if(!query.exec(command)){
        qWarning() << Q_FUNC_INFO << "Could not fill table setAlarmTemps";
    };
    emit newAlarmTemp(port, alarmLabel, val);
    emit newAlarmTempFirebase(alarmLabel);
}


QList<qreal> Localdb::getAlarmTemps(int port)
{
    QList<qreal> temps;
    QSqlQuery query;
    QString command= "SELECT * FROM setAlarmTemps WHERE port=" + QString::number(port) +";";
    qInfo() << command;
    if(!query.exec(command)){
        qWarning() << Q_FUNC_INFO << "Could not open setAlarmTemps";
    };
    if(!query.first()){
        //Log error if no query first
    }
    for(int i =0; i<3; i++){
        qreal temp = query.value(2).toReal();
        //qInfo() << Q_FUNC_INFO << query.value(2).toReal();
        setAlms[port-1][i] = temp;
        temps.append(temp);
        query.next();
    }
    qInfo() << Q_FUNC_INFO << "setAlms " << setAlms;
    return temps;
}


void Localdb::addReading(int port, qreal temp, qreal power)
{
    // Time zone map
    // Eastern - America/New_York 5h
    // Central - America/Chicago 6h
    // Mountain - America/Mexico_City 7h
    // Pacific - America/Los_Angeles 8h

    QDateTime time(QDateTime::currentDateTime());
    storeTempToDB("_min", port, temp, power);
    emit newTempReading("_min", port, time, temp, power);
    if (dbCounter == 60){
        storeTempToDB("_hour", port, temp, power);
        emit newTempReading("_hour", port, time, temp, power);
    }
    if (dbCounter == 1440){
        storeTempToDB("_day", port, temp, power);
        emit newTempReading("_day", port, time, temp, power);
    }
    if (dbCounter == 43200){
        storeTempToDB("_month", port, temp, power);
        emit newTempReading("_month", port, time, temp, power);
        dbCounter = 0;
    }
    dbCounter++;
}

void Localdb::storeTempToDB(QString db, int port, qreal temp, qreal power){
    QSqlQuery query;
    QDateTime time(QDateTime::currentDateTime());
    qint64 time_sec = time.toSecsSinceEpoch(); //qInfo() << "LOCALDB: current time, secs since epoch = " << time.toString();
    QString command = "INSERT INTO port" + QString::number(port) + db + " VALUES " + "(" + QString::number(time_sec) + "," + QString::number(temp) + "," + QString::number(power) + ");";
    query.prepare(command);
    if (!query.exec()){
        qWarning() << "LOCALDB: command = " << command;
        qWarning() << "ERROR: " << query.lastError().text(); //qInfo() << "LOCALDB: addTempReading(): added";
    }
}

QVector<dbVal> Localdb::getTemps(QString db, int port, int count){

    QVector<dbVal> timeTempsVec;
    QSqlQuery query;
    query.prepare("SELECT * FROM (SELECT * FROM port" + QString::number(port) + db  + " ORDER BY time DESC LIMIT " + QString::number(count) + ") ORDER BY time ASC");
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



