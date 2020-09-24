#include "chartdata.h"
#include "localdb.h"
#include "singleton.h"
#include <QtQuick>
#include <QVariant>



ChartData::ChartData(QObject *parent) : QObject(parent)
{    
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newTempReading, this, &ChartData::newTempRecieved);
}

void ChartData::port1BtnPressed(){
    QVector<dbVal> queryVals = Singleton<Localdb>::GetInstance().getTemps(1,6);
    QVariantList times;
    QVariantList temps;
    for(dbVal v: queryVals){
        //qInfo() << "CHARTDATA: port1BtnPressed: val.temp = " << v.temp;
        //qInfo() << "CHARTDATA: port1BtnPressed: val.time = " << v.time;
        QDateTime time;
        time.setSecsSinceEpoch(v.time);
        times.append(time);
        temps.append(v.temp);
    }

   emit portRefreshData(times, temps);

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

void ChartData::newTempRecieved(int port, QDateTime time, float temp)
{
   // qInfo() << "CHARTDATA: NEW_TEMP_RECIEVED: fired";
    switch (port)
    {
        case 1:
           // qInfo() << "CHARTDATA: newTempRecieved: in case port1:";
            emit portDataAdd(time, temp);
            //qInfo() << "CHARTDATA: emmited portDataAdd, time = " << time << " temp = " << temp;
            break;
    }

}


