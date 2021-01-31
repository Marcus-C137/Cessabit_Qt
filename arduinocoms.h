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
    bool sensorsConnected[4] = {true, true, true, true};
    bool sensorsWasConnected[4] = {true, true, true, true};
    bool initialized = false;

public slots:
    void getTemps();
    void setTemps(int port, int alarmLabel, qreal val);
    void setPortOn(int port, bool portOn);
    void activateEstop(bool value);
    void loadPortsOn(QList<bool> portsOn);
    void loadSetTemps(QVector<qreal> setTemps);
    //void startTimer();

signals:
    void tempUpdate(QVariantList temps);
    void powerUpdate(QVariantList power);
    void firebaseUpdate(QVariantList tempsAndPower);
    void firebaseAlarm(QString title, QString message);
    void newTemps();

private:
    QTimer *getTempsTimer;
    QTimer *setTempsTimer;
    float setTempers[4] = {79.9, 80.0, 80.1, 80.2};
    int fd;
    int file_i2c;
    int file_gpio;
    bool alarmSent;
    bool inAlarm;
};

#endif // ARDUINOCOMS_H
