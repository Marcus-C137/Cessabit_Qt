#ifndef ARDUINOCOMS_H
#define ARDUINOCOMS_H

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
    void startTimer();

signals:

private:
    QTimer getTempsTimer;
    QThread AC_workerThread;
    int fd;
};

#endif // ARDUINOCOMS_H
