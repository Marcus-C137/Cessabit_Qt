#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QHostAddress>
#include <QNetworkAccessManager>

class WifiManager : public QObject
{
    Q_OBJECT
public:
    explicit WifiManager(QObject *parent = nullptr);
    static QNetworkAccessManager man;
    static QHostAddress getIP();

signals:

public slots:
    void checkWifi();
    void startTimer();

private:
    QTimer timer;
    QThread wm_workerThread;

};

#endif // WIFIMANAGER_H
