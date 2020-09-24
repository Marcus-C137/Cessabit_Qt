#include "arduinocoms.h"
#include "localdb.h"
#include "singleton.h"
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
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
        //ERROR HANDLING: you can check errno to see what went wrong
        qInfo() << "Failed to open the i2c bus";
        return;
    }
    int addr = 0x08;          //<<<<<The I2C address of the slave
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
    {
        qInfo() << "Failed to acquire bus access and/or talk to slave.\n";
        //ERROR HANDLING; you can check errno to see what went wrong
        return;
    }
    getTempsTimer = new QTimer(this);
    setTempsTimer = new QTimer(this);
    connect(getTempsTimer, SIGNAL(timeout()), this, SLOT(getTemps()));
    connect(setTempsTimer, SIGNAL(timeout()), this, SLOT(setTemps()));
    getTempsTimer->start(10000);
    setTempsTimer->start(15000);

}


void ArduinoComs::getTemps()
{

    //wiringPiI2CRead(fd);
    int dataLength = 8;
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
        float currentTemps[4] = {0.0};
        QVariantList tempsToDisplay;
        for(int i= 0; i < 4; i++){
             int highBytePos = i*2;
             int lowBytePos = (i*2) + 1;
             currentTempsBuff[i] = (int16_t)((buff[highBytePos] << 8) | buff[lowBytePos]);
             currentTemps[i] = (float)currentTempsBuff[i]/10;
             tempsToDisplay.append(currentTemps[i]);
             //qInfo() << "ARDUINO_COMS: i2c currentTemps response of : " << i << " : " << currentTemps[i];
             //qInfo() << "ARDUINO_COMS: i2c tempsToDisplay of : " << i << " : " << tempsToDisplay[i];
             QMetaObject::invokeMethod(&Singleton<Localdb>::GetInstance(), "addTempReading", Qt::AutoConnection, Q_ARG(int, i+1), Q_ARG(float, currentTemps[i]));

        }
        emit tempUpdate(tempsToDisplay);

    }


}

void ArduinoComs::setTemps()
{
    int dataLength = 9;
    char buff[dataLength];
    buff[0] = 0x00; //change set Temps command

    for (int i = 0; i< 4; i++){
        setTempers[i] += 0.1;
        int16_t setTempI = (int16_t) (setTempers[i] * 10);
        //qInfo() << "ARDUINO COMMS: setTemps() - setTempsI = " << setTempI;
        buff[i*2 + 1] = (setTempI >> 8) & 0xff;
        buff[i*2 + 2] = setTempI & 0xff;
//        qInfo() << "ARDUINO COMMS: setTemps() - buff[i*2] = " <<  QString::number(buff[i*2], 16);
//        qInfo() << "ARDUINO COMMS: setTemps() - buff[i*2 +1] = " <<  QString::number(buff[i*2+1], 16);

    }

    if (write(file_i2c, buff, dataLength) != dataLength)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
    {
        /* ERROR HANDLING: i2c transaction failed */
        qInfo() << "Failed to write to the i2c bus";
    }

    //write(fd, buff, dataLength);

}


//void ArduinoComs::startTimer()
//{
//    getTempsTimer.start();
//}
