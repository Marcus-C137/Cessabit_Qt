#ifndef LOCALDB_H
#define LOCALDB_H

#include <QObject>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QPluginLoader>
#include <QVariant>
#include <QVector>

struct dbVal{
    int time;
    float temp;
};

class Localdb : public QObject
{
    Q_OBJECT


private:

    static Localdb localdb;


public:
    explicit Localdb(QObject *parent = nullptr);
    void connectDB();
    bool testPlugin();
    static void initializeDB();
    static QSqlDatabase sql_db;
    ~Localdb();
    static Localdb& Get();

    QVector<dbVal> getTemps(int port, int count);

public slots:
   void addTempReading(int port, float temp);

signals:
    void newTempReading(int port, QDateTime time, float temp);

};

#endif // LOCALDB_H
