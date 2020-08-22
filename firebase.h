#ifndef FIREBASE_H
#define FIREBASE_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QString>
#include <QDebug>
#include <QTimer>
#include "httpsworker.h"


class Firebase : public QObject
{
    Q_OBJECT
public:
    explicit Firebase(QObject *parent = nullptr);
    ~Firebase();
    void login(QString uname, QString password);
    void postCurrentTemps();
    static void doDeleteLater(HttpsWorker * obj);



signals:

public slots:
    void loginResults(QVariantMap response);
    void refreshAuthResults(QVariantMap response);
    void getSetTemps();
    void refreshAuth();

private slots:


private:

    HttpsWorker *login_httpWorker;
    HttpsWorker *refreshLogin_httpWorker;
    QTimer getTempsTimer;
    QTimer refreshAuthTimer;

    QString idToken;
    QString refreshToken;
    QString userId;
    int expiresIn;


};

#endif // FIREBASE_H
