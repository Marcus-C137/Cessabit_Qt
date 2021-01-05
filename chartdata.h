#ifndef CHARTDATA_H
#define CHARTDATA_H

#include "localdb.h"
#include <QObject>
#include <QPoint>
#include <QTimer>
#include <QVariant>
#include <QDateTime>

class ChartData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPoint tempTime READ tempTime WRITE setTempTime NOTIFY tempTimeChanged)

public:
    explicit ChartData(QObject *parent = nullptr);
    Q_INVOKABLE void loadTemps(QString db);
    Q_INVOKABLE void loadPortOn();
    Q_INVOKABLE void storeNewAlarmTemp(int alarmType, QVariant temp);
    Q_INVOKABLE void storePortOn(bool portOn);
    QPoint tempTime() const;

public slots:
    void setTempTime(QPoint tempTime);
    void newTempRecieved(QString db, int port, QDateTime time, qreal temp, qreal power);
    void newAlarmTemp(int port, int alarmLabel, qreal val);
    void newPortOn(int port, bool portOn);
    void setPort(int port){
        if (m_port == port)return;
        m_port = port;
    }

signals:
    void portTempRefresh(QVariantList times, QVariantList temps);
    void portDataAdd(QDateTime time, qreal temp);
    void tempTimeChanged(QPoint tempTime);
    void setAlarmsRefresh(QVariantList temps);
    void newAlarmTempUpdate(int alarmLabel, QVariant Val);
    void portLabelRefresh(int port);
    void portPowerUpdate(qreal power);
    void portOnRefresh(bool portOn);

private:
    QPoint m_tempTime;
    int m_port;
    QString m_db;
};

#endif // CHARTDATA_H
