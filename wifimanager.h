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
    QString macID;

signals:
    void newWifiRSSI(bool connected, float val, QString SSID);
    void networkStatusChanged(bool connected, QString wifiSSID);
    void newScannedSSIDs(QStringList SSIDs);
    void connectToWifiResults(bool connected);

public slots:
    void scanSSIDs();
    void checkWifi();
    void getWifiStrengthResult(QString result, int code);
    void connectToSSID(QString SSID, QString password);
    void connectToSSIDResult(QString result, int code);
    void connectToSSIDAgainResult(QString result, int code);
    void getMac();
    void getMacResults(QString results, int code);
    void activateConnectToSSID();
    void writeToWPAResults(QString result, int code);
    void scanSSIDsResult(QString result, int code);
    void onlineStateChanged(bool online);
    void configurationAdded(const QNetworkConfiguration &config);
    void configurationChanged(const QNetworkConfiguration &config);
    void configurationRemoved(const QNetworkConfiguration &config);
    void updateComplete();
    void resetTimer();

private:
    QTimer* wifiTimer;
    QTimer* checkWifiAgainTimer;
    QTimer* checkWifiStatusTimer;
    Process* p_getWifiStrength;
    Process* p_connectToWifi;
    Process* p_connectToWifiAgain;
    Process* p_writeToWPA;
    Process* p_scanSSIDs;
    Process* p_getMac;
    QNetworkConfigurationManager* ncm;
    QNetworkConfiguration cfg;
};

#endif // WIFIMANAGER_H
