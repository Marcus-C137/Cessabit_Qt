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

    //------SAFETY GPIO ----------
    char *filenameGPIO = (char*)"/sys/class/gpio/export";
    if((file_gpio = open(filenameGPIO, O_WRONLY)) < 0)
    {
        qInfo() << "Failed to open GPIO";
        return;
    }
    getTempsTimer = new QTimer(this);
    connect(getTempsTimer, SIGNAL(timeout()), this, SLOT(getTemps()));
    getTempsTimer->start(10000);
    connect(&Singleton<Localdb>::GetInstance(), &Localdb::newAlarmTemp , this, &ArduinoComs::setTemps);
    loadTemps(Singleton<Localdb>::GetInstance().setAlms);
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
             powerToDisplay.append(powerPercentage[i-4]);
             //qInfo() << "ARDUINO_COMS: i2c powerResponse response of : " << i-4 << " : " << powerPercentage[i-4];
             //qInfo() << Q_FUNC_INFO << "buff[highBytePos]: " << (int)buff[highBytePos];
             //qInfo() << Q_FUNC_INFO << "buff[lowBytePos]: " << (int)buff[lowBytePos];
        }

        for (int i = 0; i < 4; i++){
            QMetaObject::invokeMethod(&Singleton<Localdb>::GetInstance(), "addReading", Qt::AutoConnection, Q_ARG(int, i+1), Q_ARG(qreal, currentTemps[i]), Q_ARG(qreal, powerPercentage[i]));
        }
        tempAndPower = tempsToDisplay + powerToDisplay;
        tempsToDisplay.append(powerToDisplay);
        emit tempUpdate(tempsToDisplay);
        emit powerUpdate(powerToDisplay);
        emit firebaseUpdate(tempAndPower);
    }
}

void ArduinoComs::setTemps(int port, int alarmLabel, qreal val)
{
    qInfo() << Q_FUNC_INFO;
    if(alarmLabel == 0){
        int dataLength = 3;
        char buff[dataLength];
        switch(port){
        case 1:
            buff[0] = {0x00};
        case 2:
            buff[0] = {0x01};
        case 3:
            buff[0] = {0x02};
        case 4:
            buff[0] = {0x03};
        }
        int16_t setTempI = (int16_t) (val *10);
        //qInfo() << "ARDUINO COMMS: setTemps() - setTempsI = " << setTempI;
        buff[1] = (setTempI >> 8) & 0xff;
        buff[2] = setTempI & 0xff;
        qInfo() << "ARDUINO COMMS: setTemps() - buff[0] = " <<  QString::number(buff[0], 16);
        qInfo() << "ARDUINO COMMS: setTemps() - buff[1] = " <<  QString::number(buff[1], 16);
        qInfo() << "ARDUINO COMMS: setTemps() - buff[2] = " <<  QString::number(buff[2], 16);

        if (write(file_i2c, buff, dataLength) != dataLength)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
        {
            /* ERROR HANDLING: i2c transaction failed */
            qInfo() << "Failed to write to the i2c bus";
        }
    }
}

void ArduinoComs::loadTemps(QVector<QVector<qreal> > setAlms)
{
    for (int i = 0; i < 4; i++){
        qreal temp = setAlms[i][0];
        setTemps(i, 0, temp);
    }
}


//void ArduinoComs::startTimer()
//{
//    getTempsTimer.start();
//}
