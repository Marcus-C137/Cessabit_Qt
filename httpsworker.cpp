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
   connect(&manager, &QNetworkAccessManager::networkAccessibleChanged, this, &HttpsWorker::networkAccessibleChanged);
   connect(&manager, &QNetworkAccessManager::authenticationRequired, this, &HttpsWorker::authenticationRequired);
   //QVariantMap responseMap;
   //emit requestFinished(responseMap);
}

void HttpsWorker::get(QString URL, QVariantMap params, QVariantMap header, QVariantMap body)
{
    httpRequest _request = makeRequest(URL, params, header, body);
    QNetworkReply* reply = manager.get(_request.request);
    connect(reply, &QNetworkReply::readyRead, this, &HttpsWorker::readyRead);
    connect(&manager, &QNetworkAccessManager::finished, this, &HttpsWorker::finished, Qt::UniqueConnection);
}

void HttpsWorker::get(QString URL)
{
    QNetworkRequest req;
    req.setUrl(URL);
    QNetworkReply* reply = manager.get(req);
    connect(&manager, &QNetworkAccessManager::finished, this, &HttpsWorker::finishedDownload, Qt::UniqueConnection);
}

void HttpsWorker::post(QString URL, QVariantMap params, QVariantMap header, QVariantMap body)
{
    httpRequest _request = makeRequest(URL, params, header, body);
    QNetworkReply* reply = manager.post(_request.request, _request.body.toJson());
    connect(reply, &QNetworkReply::readyRead,this, &HttpsWorker::readyRead);
    connect(&manager, &QNetworkAccessManager::finished, this, &HttpsWorker::finished, Qt::UniqueConnection);
}

void HttpsWorker::patch(QString URL, QVariantMap params, QVariantMap header, QVariantMap body)
{

    httpRequest _request = makeRequest(URL, params, header, body);
    QNetworkReply* reply = manager.sendCustomRequest(_request.request, "PATCH", _request.body.toJson());
    connect(reply, &QNetworkReply::readyRead, this, &HttpsWorker::readyRead);
    connect(&manager, &QNetworkAccessManager::finished, this, &HttpsWorker::finished, Qt::UniqueConnection);
}

httpRequest HttpsWorker::makeRequest(QString URL, QVariantMap params, QVariantMap header, QVariantMap body){
    QMapIterator<QString, QVariant> p(params);
    QMapIterator<QString, QVariant> h(header);
    QMapIterator<QString, QVariant> b(body);
    QNetworkRequest request;

    //URL and params
    if (p.hasNext()){
        URL = URL + "?";
        while(p.hasNext()){
            p.next();
            URL = URL + p.key() + "=" + p.value().toString();
            if(p.hasNext()) URL = URL+ "&";
        }
    }
    //qInfo() << Q_FUNC_INFO << "URL: " << URL;
    request.setUrl(URL);

    //header
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    while(h.hasNext()){
        h.next();
        QString key = h.key();
        QString value = h.value().toString();
        request.setRawHeader(key.toLocal8Bit(), value.toLocal8Bit());
    }

    //body
    bodyDoc = QJsonDocument::fromVariant(body); //qInfo() << Q_FUNC_INFO << "body: " << bodyDoc.toJson();
    _request.request = request;
    _request.body = bodyDoc;
    return _request;
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
    if(reply->error()){
        qInfo() << Q_FUNC_INFO << "QNetworkReply error: " << reply->error();
        qInfo() << Q_FUNC_INFO << "QNetworkReply body: " << reply->readAll();
        qInfo() << Q_FUNC_INFO << "QNetworkRequest URL: " << _request.request.url();
        qInfo() << Q_FUNC_INFO << "QNetworkRequest body: " << _request.body;
        const QList<QByteArray>& rawHeaderList(_request.request.rawHeaderList());
        foreach (QByteArray rawHeader, rawHeaderList) {
          qInfo() << Q_FUNC_INFO << "QNetworkRequest rawHeader: " << _request.request.rawHeader(rawHeader);
        }
    }
    QByteArray responseBytes = reply->readAll();
    //qInfo() << "in HttpsWorker::finished";s
    auto responseDoc = QJsonDocument::fromJson(responseBytes);
    QJsonObject responseObj = responseDoc.object();
    QVariantMap responseMap = responseObj.toVariantMap();
    emit requestFinished(responseMap);
    return;
}

void HttpsWorker::finishedDownload(QNetworkReply *reply)
{
    reply->deleteLater();
    if(!reply) return;
    if(reply->error()){
        qInfo() << Q_FUNC_INFO << "QNetworkReply error: " << reply->error();
        qInfo() << Q_FUNC_INFO << "QNetworkReply body: " << reply->readAll();
        qInfo() << Q_FUNC_INFO << "QNetworkRequest URL: " << _request.request.url();
        qInfo() << Q_FUNC_INFO << "QNetworkRequest body: " << _request.body;
        const QList<QByteArray>& rawHeaderList(_request.request.rawHeaderList());
        foreach (QByteArray rawHeader, rawHeaderList) {
          qInfo() << Q_FUNC_INFO << "QNetworkRequest rawHeader: " << _request.request.rawHeader(rawHeader);
        }
        return;
    }
    QByteArray file = reply->readAll();
    qInfo() << "in HttpsWorker::finishedDownload";
    emit requestFinishedDownloaded(file);
    return;
}

void HttpsWorker::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    Q_UNUSED(accessible);
    qInfo() << "networkAccessibleChanged";
}

