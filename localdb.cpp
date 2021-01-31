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
#include <QObject>
#include "localdb.h"
#include "arduinocoms.h"

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
    QVector<qreal> setTemp;
    for(int i=0; i < 4; i++){
        setTemp.append(setAlms[i][0]);
    }
    emit setTempsLoaded(setTemp);
}

void Localdb::loadPortsOn(){
    getPortsOn();
    emit portsOnLoaded(portsOn);

}

void Localdb::checkDBs()
{
    QSqlQuery query(sql_db);
    QString commandPre = "create table if not exists ";
    QStringList tables = {"port1", "port2", "port3", "port4"};
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

    params = "(port INT, portOn BOOL)";
    if(!query.exec(commandPre + "portsOn" + params)){
        qWarning() << Q_FUNC_INFO << "Could not make table portsOn";
    }

    params = "(port INT, setAlarm TEXT, val REAL)";
    if(!query.exec(commandPre + "setAlarmTemps" + params)){
        qWarning() << Q_FUNC_INFO << "Could not make table setAlarmTemps";
        qWarning() << Q_FUNC_INFO << query.lastError();
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
    //Fill portsOn table
    commandPre= "INSERT INTO portsOn(port,portOn) ";
    for (int i=1; i < 5; i++){
        QString command = commandPre + "SELECT " + QString::number(i) + ",true "
                                      " WHERE NOT EXISTS("
                                      "SELECT 1 "
                                      "FROM portsOn "
                                      "WHERE port = " + QString::number(i) + ");" ;

        if(!query.exec(command)){qWarning() << Q_FUNC_INFO << "Could not fill table portsOn";};
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

void Localdb::changePortOn(int port, bool portOn)
{
    QSqlQuery query;
    QString portOnS = "";
    portOn ? portOnS = "true" : portOnS="false";
    QString command = "UPDATE portsOn SET portOn=" + portOnS + " WHERE port=" + QString::number(port);
    if(!query.exec(command)){
        qWarning() << Q_FUNC_INFO << "Could not update table PortsOn";
    };
    emit newPortOnVal(port, portOn);
}


QList<qreal> Localdb::getAlarmTemps(int port)
{
    QList<qreal> temps;
    QSqlQuery query;
    QString command= "SELECT * FROM setAlarmTemps WHERE port=" + QString::number(port) +";";
    if(!query.exec(command)) qWarning() << Q_FUNC_INFO << "Could not open setAlarmTemps " << query.lastError();
    if(!query.first()) qWarning() << Q_FUNC_INFO << "no query first " << query.lastError();
    for(int i =0; i<3; i++){
        qreal temp = query.value(2).toReal();
        setAlms[port-1][i] = temp;
        temps.append(temp);
        query.next();
    }
    return temps;
}

QList<bool> Localdb::getPortsOn()
{
    QList<bool> _portsOn;
    QSqlQuery query;
    query.prepare("SELECT * FROM portsOn");

    if (!query.exec()) qWarning() << Q_FUNC_INFO << query.lastError();
    if(!query.first()) qWarning() << Q_FUNC_INFO << "NO QUERY FIRST";

    for (int i = 0; i < 4; i++){
        bool portOn = query.value(1).toBool();
        _portsOn.append(portOn);
        query.next();
    }

    portsOn = _portsOn;
    return portsOn;
}

void Localdb::loadTemps(){
    QSqlQuery query;
    for(int port = 1; port < 5; port++){
        QVector<dbVal> timeTempsVec;
        query.prepare("SELECT * FROM port" + QString::number(port));
        if (!query.exec()) qWarning() << "ERROR: " << query.lastError().text();
        while(query.next()){
            int time  = query.value(0).toInt();
            qreal temp = query.value(1).toReal();
            qreal power = query.value(2).toReal();
            timeTempsVec.append({time, temp, power});
        }
        QString portKey = "port" +QString::number(port);
        portsTimesTemps[portKey] = timeTempsVec;

    }
//    qInfo() << Q_FUNC_INFO << "Loaded Temps";
//    QMapIterator<QString, QVector<dbVal>> i(portsTimesTemps);
//    while(i.hasNext()){
//        i.next();
//        qInfo() << Q_FUNC_INFO << i.key();
//        for(int j = 0; j< i.value().size(); j++){
//            qInfo() << Q_FUNC_INFO << i.value().at(j).power;
//        }
    //    }
}


void Localdb::addReading(int port, qreal temp, qreal power)
{
    // Time zone map
    // Eastern - America/New_York 5h
    // Central - America/Chicago 6h
    // Mountain - America/Mexico_City 7h
    // Pacific - America/Los_Angeles 8h

    QDateTime time(QDateTime::currentDateTime());
    storeTempToDB(port, temp, power);
    emit newTempReading(port, time, temp, power);
}

void Localdb::storeTempToDB(int port, qreal temp, qreal power){
    QSqlQuery query;
    QString portKey = "port"+QString::number(port);
    QDateTime time(QDateTime::currentDateTime());
    int time_sec = time.toSecsSinceEpoch(); //qInfo() << "LOCALDB: current time, secs since epoch = " << time.toString();
    QString command = "INSERT INTO port" + QString::number(port) + " VALUES " + "(" + QString::number(time_sec) + "," + QString::number(temp) + "," + QString::number(power) + ");";
    portsTimesTemps[portKey].append({time_sec, temp, power});
    query.prepare(command);
    if (!query.exec()){
        qWarning() << "LOCALDB: command = " << command;
        qWarning() << "ERROR: " << query.lastError().text(); //qInfo() << "LOCALDB: addTempReading(): added";
    }
}


void Localdb::cleanDB_Min()
{
    //CHECK IF MIN IS TOO LARGE
    //qInfo() << Q_FUNC_INFO;
    int maxDBsize = 10;
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
        //qInfo() << Q_FUNC_INFO << "LOCALDB: - size of port" + port + "_min db before delete " << query_min.value(0).toString();

        //DELETE
        int dbSize = query_min.value(0).toInt();
        query_min.finish();
        //qInfo() << "LOCALDB: cleanDB_min() - dbSize: " << QString::number(dbSize);

        if (dbSize > maxDBsize){
            QSqlQuery query_del;
            int delLimit = dbSize - maxDBsize;
            query_del.prepare("DELETE FROM " + port + " WHERE time IN (SELECT time FROM " + port + " LIMIT " + QString::number(delLimit) + ")" );
            if(!query_del.exec()) {
                qWarning() << "LOCALDB: - could not delete from database";
                qWarning() << query_del.lastError().text();
            }
            loadTemps();
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
        //qInfo() << Q_FUNC_INFO << "LOCALDB: - size of port" + port + "_min db after delete " << query_min2.value(0).toString();
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



