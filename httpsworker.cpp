#include "httpsworker.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>

HttpsWorker::HttpsWorker(QObject *parent) : QObject(parent)
{
   connect(&manager, &QNetworkAccessManager::finished, this, &HttpsWorker::finished);
   connect(&manager, &QNetworkAccessManager::networkAccessibleChanged, this, &HttpsWorker::networkAccessibleChanged);
   connect(&manager, &QNetworkAccessManager::authenticationRequired, this, &HttpsWorker::authenticationRequired);
   QVariantMap responseMap;
   emit requestFinished(responseMap);
}

void HttpsWorker::get(QString location)
{
    Q_UNUSED(location);

}

void HttpsWorker::post(QString URL, QByteArray data)
{

    QNetworkRequest request = QNetworkRequest(URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = manager.post(request, data);
    connect(reply, &QNetworkReply::readyRead,this, &HttpsWorker::readyRead);

}

void HttpsWorker::readyRead()
{

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    Q_UNUSED(reply);

}

void HttpsWorker::authenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    Q_UNUSED(authenticator);
    qInfo() << "authentication Required";

}


void HttpsWorker::finished(QNetworkReply *reply)
{
    reply->deleteLater();
    if(!reply) return;
    QByteArray responseBytes = reply->readAll();
    qInfo() << "in HttpsWorker::finished";
    auto responseDoc = QJsonDocument::fromJson(responseBytes);
    QJsonObject responseObj = responseDoc.object();
    QVariantMap responseMap = responseObj.toVariantMap();
    emit requestFinished(responseMap);
    return;

}

void HttpsWorker::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    Q_UNUSED(accessible);
    qInfo() << "networkAccessibleChanged";
}

