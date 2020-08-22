#ifndef HTTPSWORKER_H
#define HTTPSWORKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>

class HttpsWorker : public QObject
{
    Q_OBJECT
public:
    explicit HttpsWorker(QObject *parent = nullptr);

signals:
    void requestFinished(QVariantMap response);

public slots:
    void get(QString location);
    void post(QString locations, QByteArray data);

private slots:
    void readyRead();
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void finished(QNetworkReply *reply);
    void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);


private:
    QNetworkAccessManager manager;



};

#endif // HTTPSWORKER_H
