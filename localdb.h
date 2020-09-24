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



public:
    explicit Localdb(QObject *parent = nullptr);
    ~Localdb();
    void connectDB();
    void checkDBs();
    bool testPlugin();
    static void initializeDB();

    QVector<dbVal> getTemps(int port, int count);
    QSqlDatabase sql_db;
    QThread DB_workerThread;
    QTimer *cleanDBbyMin;


public slots:
   void addTempReading(int port, float temp);
   void cleanDB_Min();
   void cleanDB_Hour();
   void cleanDB_Day();
   void cleanDB_Month();
   void startTimer();


signals:
    void newTempReading(int port, QDateTime time, float temp);

};

#endif // LOCALDB_H
