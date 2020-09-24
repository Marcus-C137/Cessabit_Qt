#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkConfigurationManager>
#include "process.h"

class WifiManager : public QObject
{
    Q_OBJECT

public:
    explicit WifiManager(QObject *parent = nullptr);
    static QNetworkAccessManager man;
    QHostAddress getIP();
    void getWifiStrength();

signals:
    void newWifiRSSI(bool connected, float val, QString SSID);
    void networkStatusChanged(bool connected, QString wifiSSID);
    void newScannedSSIDs(QStringList SSIDs);

public slots:
    void scanSSIDs();
    void checkWifi();
    void getWifiStrengthResult(QString result, int code);
    void connectToSSID(QString SSID, QString password);
    void connectToSSIDResult(QString result, int code);
    void scanSSIDsResult(QString result, int code);
    void onlineStateChanged(bool online);
    void configurationAdded(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void configurationRemoved(const QNetworkConfiguration &config);
    void updateComplete();

private:
    QTimer* wifiTimer;
    Process* p_getWifiStrength;
    Process* p_connectToWifi;
    Process* p_scanSSIDs;
    QNetworkConfigurationManager* ncm;
    QNetworkConfiguration cfg;
};

#endif // WIFIMANAGER_H
