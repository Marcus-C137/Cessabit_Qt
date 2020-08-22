#include "wifimanager.h"
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QHostAddress>
#include <QNetworkInterface>

WifiManager::WifiManager(QObject *parent) : QObject(parent)
{
    moveToThread(&wm_workerThread);
    connect(&wm_workerThread, SIGNAL(started()), this, SLOT(startTimer()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(checkWifi()));
//    timer.setInterval(1000);
//    timer.moveToThread(&wm_workerThread);
//    wm_workerThread.start();

}

QHostAddress WifiManager::getIP()
{
    QNetworkInterface wlan0 = QNetworkInterface::interfaceFromName("wlan0");
    QList<QNetworkAddressEntry> wlan0Addresses = wlan0.addressEntries();
    QNetworkAddressEntry wlan0Address = wlan0Addresses.at(0);
    QHostAddress wlan0IP = wlan0Address.ip();
    qInfo() << wlan0Address.ip().toString();
    return wlan0IP;

}

void WifiManager::checkWifi()
{
    getIP();
}

void WifiManager::startTimer()
{
    qInfo() << "startingTimer";
    timer.start();
}


