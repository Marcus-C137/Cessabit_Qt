#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QProcess>
#include <QSharedPointer>
#include "wifimanager.h"
#include "process.h"

WifiManager::WifiManager(QObject *parent) : QObject(parent)
{
    wifiTimer = new QTimer(this);
    connect(wifiTimer, SIGNAL(timeout()), this, SLOT(checkWifi()));
    wifiTimer->start(10000);

    p_getWifiStrength = new Process(this);
    connect(p_getWifiStrength, &Process::output, this,  &WifiManager::getWifiStrengthResult);

    p_connectToWifi = new Process(this);
    connect(p_connectToWifi, &Process::output, this, &WifiManager::connectToSSIDResult);

    p_scanSSIDs = new Process(this);
    connect(p_scanSSIDs, &Process::output, this, &WifiManager::scanSSIDsResult);

    ncm = new QNetworkConfigurationManager(this);
    connect(ncm, &QNetworkConfigurationManager::configurationAdded, this, &WifiManager::configurationAdded);
    connect(ncm, &QNetworkConfigurationManager::configurationChanged, this, &WifiManager::configurationChanged);
    connect(ncm, &QNetworkConfigurationManager::configurationRemoved, this, &WifiManager::configurationRemoved);
    connect(ncm, &QNetworkConfigurationManager::onlineStateChanged, this, &WifiManager::onlineStateChanged);
    connect(ncm, &QNetworkConfigurationManager::updateCompleted, this, &WifiManager::updateComplete);
}

QHostAddress WifiManager::getIP()
{
    QNetworkInterface wlan0 = QNetworkInterface::interfaceFromName("wlan0");
    QList<QNetworkAddressEntry> wlan0Addresses = wlan0.addressEntries();
    QNetworkAddressEntry wlan0Address = wlan0Addresses.at(0);
    QHostAddress wlan0IP = wlan0Address.ip();
    qInfo() <<  wlan0Address.ip().toString();
    return wlan0IP;
}

void WifiManager::checkWifi()
{
    //getIP();
    getWifiStrength();

}

void WifiManager::getWifiStrength()
{

  QString command = "iwconfig";
  QStringList args;
  args << "wlan0";
  p_getWifiStrength->start(command, args);

}

void WifiManager::getWifiStrengthResult(QString result, int code)
{
    bool connected = false;

    if(code !=0 ){
        qInfo() << Q_FUNC_INFO << "code not equal to zero";
        emit newWifiRSSI(false, 0, "");
        return;
    }

    int start_indx = result.indexOf("Link Quality=");

    if (start_indx == -1){
        emit newWifiRSSI(false, 0, "");
        return;
    }else{
        connected = true;
    }

    int end_indx = result.indexOf("/70");
    int start_indx_end = start_indx + 13;
    QStringRef subString(&result, start_indx_end, end_indx-start_indx_end);
    float RSSIval = subString.toFloat()/70;

    int start_i = result.indexOf("ESSID:");
    int start_i_off = start_i+6;
    int firstQuote = result.indexOf("\"", start_i_off);
    int secondQuote = result.indexOf("\"", start_i_off+1);
    QString SSID = result.mid(firstQuote+1, secondQuote-firstQuote-1);

    emit newWifiRSSI(connected, RSSIval, SSID);
}


void WifiManager::connectToSSID(QString SSID, QString password)
{

}

void WifiManager::connectToSSIDResult(QString result, int code)
{
    //TODO: error if cannot convert to number or if indexOf returns 0;

    //qInfo() << Q_FUNC_INFO << result << " Code " << code;
    int start_indx = result.indexOf("Link Quality=");
    int end_indx = result.indexOf("/70");
    int start_indx_end = start_indx + 13;
    QStringRef subString(&result, start_indx_end, end_indx-start_indx_end);
    //qInfo() << Q_FUNC_INFO << subString;
    int RSSIval = subString.toInt();
    //emit newWifiRSSI(RSSIval);
}


void WifiManager::scanSSIDs()
{
    QString command = "sudo";
    QStringList args;
    args << "iwlist"<< "wlan0" << "scan";
    p_scanSSIDs->start(command, args);
}

void WifiManager::scanSSIDsResult(QString result, int code)
{
    //qInfo() << Q_FUNC_INFO << result;
    qInfo() << "in scanSSIDsResult";
    qInfo() << "Code : " << code;
    qInfo() << Q_FUNC_INFO << result.count("ESSID:");
    QStringList SSIDs;
    int start_indx = 0;
    for(int i=0; i<result.count("ESSID:"); i++)
    {
        start_indx = result.indexOf("ESSID:\"", start_indx);
        int ESSID_length = 7;
        int end_indx = result.indexOf("\"", start_indx+ESSID_length);
        int start_indx_end = start_indx + ESSID_length;
        start_indx = end_indx + 1;
        QString SSID = result.mid(start_indx_end, end_indx-start_indx_end);
        qInfo() << Q_FUNC_INFO << SSID;
        if (!SSID.isEmpty()) SSIDs.append(SSID);

    }
    emit newScannedSSIDs(SSIDs);
}


void WifiManager::onlineStateChanged(bool online)
{
    qInfo() << Q_FUNC_INFO;

}

void WifiManager::configurationAdded(const QNetworkConfiguration &config)
{
    qInfo() << Q_FUNC_INFO;
}

void WifiManager::configurationChanged(const QNetworkConfiguration &config)
{
    qInfo() << Q_FUNC_INFO;
}

void WifiManager::configurationRemoved(const QNetworkConfiguration &config)
{
    qInfo() << Q_FUNC_INFO;
}

void WifiManager::updateComplete()
{
    //qInfo() << Q_FUNC_INFO;
}


