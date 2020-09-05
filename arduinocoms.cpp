#include "arduinocoms.h"
#include "wiringPi.h"
#include "localdb.h"
#include <unistd.h>
#include <wiringPiI2C.h>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QMetaObject>



ArduinoComs::ArduinoComs(QObject *parent) : QObject(parent)
{
    moveToThread(&AC_workerThread);
    wiringPiSetup();
    fd = wiringPiI2CSetup(8);
    qInfo() << "ARDUINO_COMMS: fd: " << fd;
    connect(&AC_workerThread, SIGNAL(started()), this, SLOT(startTimer()));
    connect(&getTempsTimer, SIGNAL(timeout()), this, SLOT(getTemps()));
    getTempsTimer.setInterval(10000);
    getTempsTimer.moveToThread(&AC_workerThread);
    AC_workerThread.start();

}


void ArduinoComs::getTemps()
{

    //wiringPiI2CRead(fd);
    int dataLength = 8;
    char buff[dataLength];
    read(fd, buff, dataLength);
    int16_t currentTempsBuff[4] = {0};
    float currentTemps[4] = {0.0};
    for(int i= 0; i < 4; i++){
         int highBytePos = i*2;
         int lowBytePos = (i*2) + 1;
         currentTempsBuff[i] = (int16_t)((buff[highBytePos] << 8) | buff[lowBytePos]);
         currentTemps[i] = (float)currentTempsBuff[i]/10;
         //qInfo() << "ARDUINO_COMS: i2c response of : " << i << " : " << currentTemps[i];
        QMetaObject::invokeMethod(&Localdb::Get(), "addTempReading", Qt::AutoConnection, Q_ARG(int, i+1), Q_ARG(float, currentTemps[i]));
    }
}


void ArduinoComs::startTimer()
{
    getTempsTimer.start();
}
