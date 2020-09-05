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
    Q_INVOKABLE void port1BtnPressed();
    QPoint tempTime() const;

public slots:
    void setTempTime(QPoint tempTime);
    void newTempRecieved(int port, QDateTime time, float temp);

signals:
    void portRefreshData(QVariantList times, QVariantList temps);
    void portDataAdd(QDateTime time, float temp);
    void tempTimeChanged(QPoint tempTime);

private:
    QPoint m_tempTime;
};

#endif // CHARTDATA_H
