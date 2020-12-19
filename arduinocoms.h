#ifndef ARDUINOCOMS_H
#define ARDUINOCOMS_H
#include "localdb.h"
#include <QObject>
#include <QTimer>
#include <QThread>

class ArduinoComs : public QObject
{
    Q_OBJECT
public:
    explicit ArduinoComs(QObject *parent = nullptr);

public slots:
    void getTemps();
    void setTemps(int port, int alarmLabel, qreal val);
    //void startTimer();

signals:
    void tempUpdate(QVariantList temps);
    void powerUpdate(QVariantList power);
    void firebaseUpdate(QVariantList tempsAndPower);
    void newTemps();

private:
    void loadTemps(QVector<QVector<qreal>> setAlms);
    QTimer *getTempsTimer;
    QTimer *setTempsTimer;
    float setTempers[4] = {79.9, 80.0, 80.1, 80.2};
    int fd;
    int file_i2c;
    int file_gpio;
};

#endif // ARDUINOCOMS_H
