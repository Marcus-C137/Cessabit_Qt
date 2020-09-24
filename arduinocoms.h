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
    void setTemps();
    //void startTimer();

signals:
    void tempUpdate(QVariantList temps);

private:
    QTimer *getTempsTimer;
    QTimer *setTempsTimer;
    int fd;
    float setTempers[4] = {79.9, 80.0, 80.1, 80.2};
    int file_i2c;
};

#endif // ARDUINOCOMS_H
