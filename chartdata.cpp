#include "chartdata.h"
#include "localdb.h"
#include "singleton.h"
#include <QtQuick>
#include <QVariant>



ChartData::ChartData(QObject *parent) : QObject(parent)
{    
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newTempReading, this, &ChartData::newTempRecieved);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newAlarmTemp, this, &ChartData::newAlarmTemp);

}

void ChartData::loadTemps(QString db){
    m_db = db;
    qInfo() << "m_db = " << m_db;
    qint64 timeInterval = 0;
    QVector<dbVal> queryVals = Singleton<Localdb>::GetInstance().getTemps(db, m_port,6);
    if (db == "_min"){
        timeInterval = 60/6;
    }else if (db == "_hour"){
        timeInterval = 3600/6;
    }else if (db == "_day"){
        timeInterval = 86400/6;
    }else if (db == "_month"){
        timeInterval = 2592000/6;
    }else{
        qWarning() << Q_FUNC_INFO << "db does not match value. You fucked up";
    }
    QVariantList times;
    QVariantList temps;
    QDateTime dbTime;
    QDateTime acceptedTimeMin;
    QDateTime acceptedTimeMax;
    QDateTime intervalTime;
    QList<QDateTime> dbTimeList;
    QList<QDateTime> intervalTimeList;
    int size = queryVals.size();
    int counter = 0;
    int index = 0;

    for(dbVal v: queryVals){
        counter++;
        index = size - counter; //qInfo() << Q_FUNC_INFO << "index: " << index;
        dbTime.setSecsSinceEpoch(v.time);
        //qInfo() << Q_FUNC_INFO << "dbTime: " << dbTime.toSecsSinceEpoch();
        intervalTime.setSecsSinceEpoch(QDateTime::currentSecsSinceEpoch()-(timeInterval * index));
        //qInfo() << Q_FUNC_INFO << "intervalTime: " << intervalTime.toSecsSinceEpoch();
        dbTimeList.push_back(dbTime);
        intervalTimeList.push_back(intervalTime);
    }

    int i = 0;
    for(dbVal v: queryVals){
        qInfo() << Q_FUNC_INFO << "i: " << i;
        acceptedTimeMax = intervalTimeList[i].addSecs(timeInterval/2);  qInfo() << Q_FUNC_INFO << "acceptedTimeMax: " << acceptedTimeMax.toSecsSinceEpoch();
        acceptedTimeMin = intervalTimeList[i].addSecs(-timeInterval/2);  qInfo() << Q_FUNC_INFO << "acceptedTimeMin: " << acceptedTimeMin.toSecsSinceEpoch();
        for(int j=0; j < size; j++){
            qInfo() << Q_FUNC_INFO << "dbTimeList[index]" << dbTimeList[j].toSecsSinceEpoch();
            if (acceptedTimeMin <= dbTimeList[j] && dbTimeList[j] < acceptedTimeMax){
                (v.temp < -195) ? temps.append(0.0) : temps.append(v.temp);
                times.append(dbTimeList[j]);
                break;
            }
            if (j == size - 1){
                times.append(intervalTimeList[i]);
                temps.append(0.0);
            }
        }
        i++;
    }


    QList<qreal> alarmTemps = Singleton<Localdb>::GetInstance().getAlarmTemps(m_port);
    QVariantList alarmTempsEmit;
    for(int i=0; i<3; i++) alarmTempsEmit.append(alarmTemps[i]);
    //qInfo() <<  Q_FUNC_INFO << "alarm Temps Emit" << alarmTempsEmit;
    emit portTempRefresh(times, temps);
    emit setAlarmsRefresh(alarmTempsEmit);
    emit portLabelRefresh(m_port);


}

void ChartData::storeNewAlarmTemp(int alarmType, QVariant temp)
{
    Singleton<Localdb>::GetInstance().changeAlarmTemp(m_port, alarmType, temp.toReal());
}

QPoint ChartData::tempTime() const
{
    return m_tempTime;
}

void ChartData::setTempTime(QPoint tempTime)
{
    if (m_tempTime == tempTime)
        return;

    m_tempTime = tempTime;
    emit tempTimeChanged(m_tempTime);
}

void ChartData::newTempRecieved(QString db, int port, QDateTime time, qreal temp, qreal power)
{
    if (db != "_min") qInfo() << "CHARTDATA: newTempRecieved: db " << db << " port " << port;
    if (m_port == port && m_db == db){
        if (temp < -195) temp = 0.0;
        emit portDataAdd(time, temp);
        emit portPowerUpdate(power);
    }
    //qInfo() << "CHARTDATA: emmited portDataAdd for" << port << " time = " << time << " temp = " << temp;

}

void ChartData::newAlarmTemp(int port, int alarmLabel, qreal val)
{
    if(port == m_port){
        QVariant Val = QVariant(val);
        emit newAlarmTempUpdate(alarmLabel,val);
    }
}


