#ifndef HTTPSWORKER_H
#define HTTPSWORKER_H

#include <QObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QMap>

struct httpRequest{
    QNetworkRequest request;
    QJsonDocument body;
};


class HttpsWorker : public QObject
{
    Q_OBJECT

public:
    explicit HttpsWorker(QObject *parent = nullptr);


signals:
    void requestFinished(QVariantMap response);
    void requestFinishedDownloaded(QByteArray file);

public slots:
    void get(QString URL, QVariantMap params, QVariantMap header, QVariantMap body);
    void get(QString URL);
    void post(QString URL, QVariantMap params, QVariantMap header, QVariantMap body);
    void patch(QString URL, QVariantMap params, QVariantMap header, QVariantMap body);

private slots:
    void readyRead();
    void authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void finished(QNetworkReply *reply);
    void finishedDownload(QNetworkReply *reply);
    void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);


private:
    QNetworkAccessManager manager;
    QJsonDocument bodyDoc;
    httpRequest _request;
    httpRequest makeRequest(QString URL, QVariantMap params, QVariantMap header, QVariantMap body);
};

#endif // HTTPSWORKER_H
