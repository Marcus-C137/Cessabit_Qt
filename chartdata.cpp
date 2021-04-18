#include "chartdata.h"
#include "localdb.h"
#include "singleton.h"
#include <QtQuick>
#include <QVariant>



ChartData::ChartData(QObject *parent) : QObject(parent)
{    
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newTempReading, this, &ChartData::newTempRecieved);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newAlarmTemp, this, &ChartData::newAlarmTemp);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newPortOnVal, this, &ChartData::newPortOn);

}

void ChartData::loadTemps(int timeInterval_sec){
    QTime timeStart = QTime::currentTime();
    int chartSize = 6;
    QMap<QString, QVector<dbVal>> portsTimesTemps = Singleton<Localdb>::GetInstance().portsTimesTemps;
    QString portKey = "port"+QString::number(m_port);
    QVector<dbVal> dbVals = portsTimesTemps[portKey];
    QVariantList times;
    QVariantList temps;
    QDateTime dbTime;
    QDateTime acceptedTimeMin;
    QDateTime acceptedTimeMax;
    QDateTime intervalTime;
    QList<int> dbTimeList;
    QList<QDateTime> intervalTimeList;

    if (dbVals.size() < chartSize) chartSize = dbVals.size();
    // make integer list from dbvals map's vectors values
    for (dbVal v: dbVals){
        dbTimeList.append(v.time);
    }
    //make time list starting at current time "timeInterval_sec" apart
    for(int i =0; i < chartSize; i++){
        intervalTime.setSecsSinceEpoch(QDateTime::currentSecsSinceEpoch()-(timeInterval_sec * (i+1))); //qInfo() << Q_FUNC_INFO << "intervalTime: " << intervalTime.toSecsSinceEpoch();
        intervalTimeList.push_back(intervalTime);
    }
    //compare two and fill missing times with 0 vals.
    int dbValsIndex = dbVals.size() - 1;
    for(int i=0; i < chartSize; i++){
        //qInfo() << Q_FUNC_INFO << "i: " << i;
        qInfo() << Q_FUNC_INFO << "interval time list of " << i << " = " << intervalTimeList[i].toSecsSinceEpoch();
        acceptedTimeMax = intervalTimeList[i].addSecs(timeInterval_sec/2);  //qInfo() << Q_FUNC_INFO << "acceptedTimeMax: " << acceptedTimeMax.toSecsSinceEpoch();
        acceptedTimeMin = intervalTimeList[i].addSecs(-timeInterval_sec/2);  //qInfo() << Q_FUNC_INFO << "acceptedTimeMin: " << acceptedTimeMin.toSecsSinceEpoch();
        for(int j=dbValsIndex; j > 0; j = j - (timeInterval_sec/10)){
            //qInfo() << Q_FUNC_INFO << "dbTimeList " << j << " = " << dbTimeList[j];
            if (acceptedTimeMin.toSecsSinceEpoch() <= dbTimeList[j] && dbTimeList[j] <= acceptedTimeMax.toSecsSinceEpoch()){
                (dbVals[j].temp < -195) ? temps.append(0.0) : temps.append(dbVals[j].temp);
                times.append(dbTimeList[j]);
                dbValsIndex = j-1;
                break;
            }
            if (j < (timeInterval_sec/10) || dbTimeList[j] < acceptedTimeMin.toSecsSinceEpoch()){
                times.append(intervalTimeList[i].toSecsSinceEpoch());
                temps.append(0.0);
                break;
            }
        }
    }
    std::reverse(std::begin(times), std::end(times));
    std::reverse(std::begin(temps), std::end(temps));
    QList<qreal> alarmTemps = Singleton<Localdb>::GetInstance().getAlarmTemps(m_port);
    QVariantList alarmTempsEmit;
    for(int i=0; i<3; i++) alarmTempsEmit.append(alarmTemps[i]);
    //qInfo() <<  Q_FUNC_INFO << "alarm Temps Emit" << alarmTempsEmit;
    qInfo() << "times: " << times;
    qInfo() << "temps: " << temps;
    emit portTempRefresh(times, temps);
    emit setAlarmsRefresh(alarmTempsEmit);
    emit portLabelRefresh(m_port);
}

void ChartData::loadPortOn(){
    QList<bool> portsOn = Singleton<Localdb>::GetInstance().getPortsOn();
    emit portOnRefresh(portsOn[m_port-1]);
}

void ChartData::loadGain(){
    QList<int> gains = Singleton<Localdb>::GetInstance().getGains();
    emit gainRefresh(gains[m_port-1]);
}


void ChartData::storeNewAlarmTemp(int alarmType, QVariant temp)
{
    qInfo() << Q_FUNC_INFO;
    Singleton<Localdb>::GetInstance().changeAlarmTemp(m_port, alarmType, temp.toReal());
}
void ChartData::storePortOn(bool portOn)
{
    qInfo() << Q_FUNC_INFO << "storing port on";
    Singleton<Localdb>::GetInstance().changePortOn(m_port, portOn);
}

void ChartData::storeGain(int gain)
{
    qInfo() << Q_FUNC_INFO << "Storing gain" << QString::number(gain);
    Singleton<Localdb>::GetInstance().changeGain(m_port,gain);

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

void ChartData::newTempRecieved(int port, QDateTime time, qreal temp, qreal power)
{
    if (m_port == port && m_chartLive == true){
        if (temp < -195) temp = 0.0;
        emit portDataAdd(time.toSecsSinceEpoch(), temp);
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

void ChartData::newPortOn(int port, bool portOn)
{
    if(port == m_port){
        emit portOnRefresh(portOn);
    }
}


