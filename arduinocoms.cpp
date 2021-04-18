#include "arduinocoms.h"
#include "localdb.h"
#include "singleton.h"
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <sys/stat.h>
#include <sys/types.h>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QMetaObject>


ArduinoComs::ArduinoComs(QObject *parent) : QObject(parent)
{

    //----- OPEN THE I2C BUS -----
    char *filename = (char*)"/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDWR)) < 0)
    {
        qInfo() << "Failed to open the i2c bus";
        return;
    }
    int addr = 0x08;          //<<<<<The I2C address of the slave
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
    {
        qInfo() << "Failed to acquire bus access and/or talk to slave.\n";
        return;
    }

    // safety

    int fd = open("/sys/class/gpio/export", O_WRONLY);
    if (fd == -1) {
        qInfo() << Q_FUNC_INFO << "Unable to open /sys/class/gpio/export";
    }

    if (write(fd, "27", 2) != 2) {
        qInfo() << Q_FUNC_INFO << "Error writing to /sys/class/gpio/export";

    }

    close(fd);

    // Set the pin to be an output by writing "out" to /sys/class/gpio/gpio27/direction

    fd = open("/sys/class/gpio/gpio27/direction", O_WRONLY);
    if (fd == -1) {
        qInfo() << Q_FUNC_INFO << "Unable to open /sys/class/gpio/gpio27/direction";

    }

    if (write(fd, "out", 3) != 3) {
        qInfo() << Q_FUNC_INFO << "Error writing to /sys/class/gpio/gpio27/direction";

    }

    close(fd);

    activateEstop(false);
    getTempsTimer = new QTimer(this);
    connect(getTempsTimer, SIGNAL(timeout()), this, SLOT(getTemps()));
    getTempsTimer->start(10000);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newAlarmTemp , this, &ArduinoComs::setTemps);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newPortOnVal , this, &ArduinoComs::setPortOn);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newGainVal , this, &ArduinoComs::setGain);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::portsOnLoaded, this, &ArduinoComs::loadPortsOn);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::portGainsLoaded, this, &ArduinoComs::loadGains);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::setTempsLoaded, this, &ArduinoComs::loadSetTemps);
    inAlarm = false;
    alarmSent = false;
}



void ArduinoComs::getTemps()
{

    //wiringPiI2CRead(fd);
    int dataLength = 16;
    char buff[dataLength];
    //----- READ BYTES -----
    if (read(file_i2c, buff, dataLength) != dataLength)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        //ERROR HANDLING: i2c transaction failed
        printf("Failed to read from the i2c bus.\n");
    }
    else
    {
        int16_t currentTempsBuff[4] = {0};
        int16_t powerPercentageBuff[4] = {0};
        qreal currentTemps[4] = {0.0};
        qreal powerPercentage[4] = {0.0};
        QVariantList tempsToDisplay;
        QVariantList powerToDisplay;
        QVariantList tempAndPower;
        for(int i= 0; i < 4; i++){
             int highBytePos = i*2;
             int lowBytePos = (i*2) + 1;
             currentTempsBuff[i] = (int16_t)((buff[highBytePos] << 8) | buff[lowBytePos]);
             currentTemps[i] = (qreal)currentTempsBuff[i]/10;
             tempsToDisplay.append(currentTemps[i]);
             currentTemps[i] < 0 ? sensorsConnected[i] = false : sensorsConnected[i] = true;
             //qInfo() << "ARDUINO_COMS: i2c currentTemps response of : " << i << " : " << currentTemps[i];
             //qInfo() << Q_FUNC_INFO << "buff[highBytePos]: " << (int)buff[highBytePos];
             //qInfo() << Q_FUNC_INFO << "buff[lowBytePos]: " << (int)buff[lowBytePos];
             //qInfo() << "ARDUINO_COMS: i2c tempsToDisplay of : " << i << " : " << tempsToDisplay[i];
        }

        for(int i= 4; i < 8; i++){
             int highBytePos = i*2;
             int lowBytePos = (i*2) + 1;
             powerPercentageBuff[i-4] = (int16_t)((buff[highBytePos] << 8) | buff[lowBytePos]);
             powerPercentage[i-4] = qFabs((qreal)(powerPercentageBuff[i-4]-512)/512);
             if (powerPercentage[i-4] < 0.02) powerPercentage[i-4] = 0.0;
             powerToDisplay.append(powerPercentage[i-4]);
             //qInfo() << "ARDUINO_COMS: i2c powerResponse response of : " << i-4 << " : " << powerPercentage[i-4];
             //qInfo() << Q_FUNC_INFO << "buff[highBytePos]: " << (int)buff[highBytePos];
             //qInfo() << Q_FUNC_INFO << "buff[lowBytePos]: " << (int)buff[lowBytePos];
        }

        for (int i = 0; i < 4; i++){
            QMetaObject::invokeMethod(&Singleton<Localdb>::GetInstance(), "addReading", Qt::AutoConnection, Q_ARG(int, i+1), Q_ARG(qreal, currentTemps[i]), Q_ARG(qreal, powerPercentage[i]));
        }
        if(!initialized){
            initialized = true;
            for (int i=0; i<4; i++) sensorsWasConnected[i] = sensorsConnected[i];
        }else{
            QString alarmTitle = "";
            QString alarmMessage = "";
            QVector<QVector<qreal>> setAlms = Singleton<Localdb>::GetInstance().setAlms;
            QList<bool> portsOn = Singleton<Localdb>::GetInstance().portsOn;
            for(int i = 0; i<4; i++){
                qreal highAlarm = setAlms[i][1];
                qreal lowAlarm = setAlms[i][2];
                //qInfo() << Q_FUNC_INFO << "Port " + QString::number(i) + " is at " + QString::number(currentTemps[i]) + " highAlarm = " + QString::number(highAlarm) + "  ,  lowAlarm= " + QString::number(lowAlarm);
                if ((currentTemps[i] > highAlarm || currentTemps[i] < lowAlarm) && portsOn[i] && sensorsConnected[i]){
                    qreal temp;
                    currentTemps[i] < 0 ? temp = 0 : temp = currentTemps[i];
                    alarmTitle = "Cessabit Alert";
                    alarmMessage += "Port " + QString::number(i+1) + " is at " + QString::number(temp) + "F \r\n";
                }
                if(sensorsWasConnected[i] && !sensorsConnected[i] && portsOn[i]){
                    alarmTitle = "Cessabit Alert";
                    alarmMessage += "Port " + QString::number(i+1) + " sensor has become disconnected \r\n";
                }
                sensorsWasConnected[i] = sensorsConnected[i];
            }

            (alarmTitle.size() > 1) ? inAlarm = true : inAlarm = false;
            if (!inAlarm && alarmSent) alarmSent = false; //reset alarm
            if(inAlarm && !alarmSent){
                alarmSent = true;
                qInfo() << Q_FUNC_INFO << "sending firebase message " << alarmMessage;
                emit firebaseAlarm(alarmTitle, alarmMessage);
            }
            tempAndPower = tempsToDisplay + powerToDisplay;
            tempsToDisplay.append(powerToDisplay);
            emit tempUpdate(tempsToDisplay);
            emit powerUpdate(powerToDisplay);
            emit firebaseUpdate(tempAndPower);
         }
    }
}

void ArduinoComs::setTemps(int port, int alarmLabel, qreal val)
{
    qInfo() << Q_FUNC_INFO << "port " << QString::number(port) << " val " << QString::number(val);
    if(alarmLabel == 0){
        int dataLength = 3;
        char buff[dataLength];
        switch(port){
        case 1:
            buff[0] = {0x00};
            break;
        case 2:
            buff[0] = {0x01};
            break;
        case 3:
            buff[0] = {0x02};
            break;
        case 4:
            buff[0] = {0x03};
            break;
        }
        int16_t setTempI = (int16_t) (val *10);
        //qInfo() << "ARDUINO COMMS: setTemps() - setTempsI = " << setTempI;
        buff[1] = (setTempI >> 8) & 0xff;
        buff[2] = setTempI & 0xff;
//        qInfo() << "ARDUINO COMMS: setTemps() - buff[0] = " <<  QString::number(buff[0], 16);
//        qInfo() << "ARDUINO COMMS: setTemps() - buff[1] = " <<  QString::number(buff[1], 16);
//        qInfo() << "ARDUINO COMMS: setTemps() - buff[2] = " <<  QString::number(buff[2], 16);

        if (write(file_i2c, buff, dataLength) != dataLength)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
        {
            /* ERROR HANDLING: i2c transaction failed */
            qInfo() << Q_FUNC_INFO << "Failed to write to the i2c bus";
        }
    }
}

void ArduinoComs::setPortOn(int port, bool portOn)
{
    qInfo() << Q_FUNC_INFO << " port " << QString::number(port) << " port on = " << portOn;
    int dataLength = 2;
    char buff[dataLength];
    switch(port){
    case 1:
        buff[0] = {0x04};
        break;
    case 2:
        buff[0] = {0x05};
        break;
    case 3:
        buff[0] = {0x06};
        break;
    case 4:
        buff[0] = {0x07};
        break;
    }
    int8_t val;
    portOn ? val = 1 : val = 0;
    buff[1] = val;
    QByteArray buffer;
    buffer.append(buff[0]);
    buffer.append(buff[1]);
    qInfo() << Q_FUNC_INFO << " sending " << buffer.toHex();
    if (write(file_i2c, buff, dataLength) != dataLength)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        /* ERROR HANDLING: i2c transaction failed */
        qInfo() << Q_FUNC_INFO << "Failed to write to the i2c bus";
    }
}

void ArduinoComs::setGain(int port, int gain)
{
    qInfo() << Q_FUNC_INFO << " port " << QString::number(port) << " gain = " << gain;
    int dataLength = 3;
    char buff[dataLength];
    switch(port){
    case 1:
        buff[0] = {0x08};
        break;
    case 2:
        buff[0] = {0x09};
        break;
    case 3:
        buff[0] = {0x0A};
        break;
    case 4:
        buff[0] = {0x0B};
        break;
    }
    int16_t gainB = (int16_t) (gain);
    //qInfo() << "ARDUINO COMMS: setTemps() - setTempsI = " << setTempI;
    buff[1] = (gainB >> 8) & 0xff;
    buff[2] = gainB & 0xff;
//    qInfo() << "ARDUINO COMMS: setTemps() - buff[0] = " <<  QString::number(buff[0], 16);
//    qInfo() << "ARDUINO COMMS: setTemps() - buff[1] = " <<  QString::number(buff[1], 16);
//    qInfo() << "ARDUINO COMMS: setTemps() - buff[2] = " <<  QString::number(buff[2], 16);
    if (write(file_i2c, buff, dataLength) != dataLength)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        /* ERROR HANDLING: i2c transaction failed */
        qInfo() << Q_FUNC_INFO << "Failed to write to the i2c bus";
    }
}



void ArduinoComs::loadPortsOn(QList<bool> portsOn)
{
    for (int i = 0; i < portsOn.size(); i++){
        setPortOn(i+1, portsOn[i]);
    }
}

void ArduinoComs::loadSetTemps(QVector<qreal> setTemperatures)
{
    for (int i = 0; i < 4; i++){
        setTemps(i+1, 0, setTemperatures[i]);
    }
}

void ArduinoComs::loadGains(QList<int> gains)
{
    for (int i=0; i < gains.size(); i++){
        setGain(i+1, gains[i]);
    }
}

void ArduinoComs::activateEstop(bool value)
{
    //qInfo() << Q_FUNC_INFO << "estop activated ";
    fd = open("/sys/class/gpio/gpio27/value", O_WRONLY);
    if (fd == -1) {
        qInfo() << Q_FUNC_INFO << "Unable to open /sys/class/gpio/gpio27/value";
    }
    if (value == true){
        if (write(fd, "1", 1) != 1) {
            qInfo() << Q_FUNC_INFO <<"Error writing to /sys/class/gpio/gpio27/value";
        }
    }else{
        if (write(fd, "0", 1) != 1) {
            qInfo() << Q_FUNC_INFO <<"Error writing to /sys/class/gpio/gpio27/value";
        }
    }
    close(fd);

}



//void ArduinoComs::startTimer()
//{
//    getTempsTimer.start();
//}
