#ifndef LOCALDB_H
#define LOCALDB_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include <QVector>
#include <QTimer>

struct dbVal{
    int time;
    qreal temp;
    qreal power;
};

class Localdb : public QObject
{
    Q_OBJECT


private:
//    QTimer *cleanDBbyHour;
//    QTimer *cleanDBbyDay;
//    QTimer *cleanDBbyMonth;

public:
    explicit Localdb(QObject *parent = nullptr);
    ~Localdb();
    static void initializeDB();
    void connectDB();
    void checkDBs();
    void addUserInfo(QString uname, QString password);
    void changeAlarmTemp(int port, int alarmLabel, qreal val);
    void changePortOn(int port, bool portOn);
    void loadAlarmTemps();
    void loadPortsOn();
    void loadTemps();
    bool testPlugin();
    QStringList checkUserAccount();
    QList<qreal> getAlarmTemps(int port);
    QSqlDatabase sql_db;
    QThread DB_workerThread;
    QTimer *cleanDBbyMin;
    QVector<QVector<qreal>> setAlms;
    QList<bool> getPortsOn();
    QList<bool> portsOn;
    QMap<QString, QVector<dbVal>> portsTimesTemps;


public slots:
   void addReading(int port, qreal temp, qreal power);
   void storeTempToDB(int port, qreal temp, qreal power);
   void cleanDB_Min();
   void cleanDB_Hour();
   void cleanDB_Day();
   void cleanDB_Month();
   void startTimer();


signals:
    void newTempReading(int port, QDateTime time, qreal temp, qreal power);
    void newAlarmTemp(int port, int alarmLabel, qreal val);
    void newAlarmTempFirebase(int alarmLabel);
    void newPortOnVal(int port, bool portOn);
    void portsOnLoaded(QList<bool> portsOn);
    void setTempsLoaded(QVector<qreal> setTemps);

};

#endif // LOCALDB_H
