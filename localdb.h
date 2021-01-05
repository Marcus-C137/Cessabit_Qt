#ifndef LOCALDB_H
#define LOCALDB_H

#include <QObject>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include <QVector>
#include <QTimer>

struct dbVal{
    int time;
    float temp;
};

class Localdb : public QObject
{
    Q_OBJECT


private:

//    QTimer *cleanDBbyHour;
//    QTimer *cleanDBbyDay;
//    QTimer *cleanDBbyMonth;
    unsigned long int dbCounter; //1 - _min, 60 - _hour, 1440 - _day, 43200 - _month



public:
    explicit Localdb(QObject *parent = nullptr);
    ~Localdb();
    void connectDB();
    void checkDBs();
    void addUserInfo(QString uname, QString password);
    void changeAlarmTemp(int port, int alarmLabel, qreal val);
    void changePortOn(int port, bool portOn);
    void loadAlarmTemps();
    void loadPortsOn();
    static void initializeDB();
    bool testPlugin();
    QStringList checkUserAccount();
    QList<qreal> getAlarmTemps(int port);
    QVector<dbVal> getTemps(QString db, int port, int count);
    QSqlDatabase sql_db;
    QThread DB_workerThread;
    QTimer *cleanDBbyMin;
    QVector<QVector<qreal>> setAlms;
    QList<bool> getPortsOn();
    QList<bool> portsOn;


public slots:
   void addReading(int port, qreal temp, qreal power);
   void storeTempToDB(QString db, int port, qreal temp, qreal power);
   void cleanDB_Min();
   void cleanDB_Hour();
   void cleanDB_Day();
   void cleanDB_Month();
   void startTimer();


signals:
    void newTempReading(QString db, int port, QDateTime time, qreal temp, qreal power);
    void newAlarmTemp(int port, int alarmLabel, qreal val);
    void newAlarmTempFirebase(int alarmLabel);
    void newPortOnVal(int port, bool portOn);

};

#endif // LOCALDB_H
