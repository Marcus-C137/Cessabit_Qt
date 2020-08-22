#include "firebase.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QDebug>
#include <QSharedPointer>
#include "httpsworker.h"



Firebase::Firebase(QObject *parent) : QObject(parent)
{
    qInfo()<< "In firebasde constructor";
    idToken = "";
    refreshToken = "";
    userId = "";
    expiresIn = 0;

    login_httpWorker = new HttpsWorker();
    refreshLogin_httpWorker= new HttpsWorker();
    bool login_connected = connect(login_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(loginResults(QVariantMap)));
    qInfo() << "login slot signal connected: " << login_connected;
    bool refresh_connected = connect(refreshLogin_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(refreshAuthResults(QVariantMap)));
    qInfo() << "refreshAuth() signal slot connected: " << refresh_connected;
}

Firebase::~Firebase()
{
    qInfo() << "firebase destroyed";
}


void Firebase::login(QString uname, QString password)
{
    qInfo() << "in firebase::login";
    QString URL = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=AIzaSyDOuF3bf6XnwzKZZoYX7tT_nV4JEI8IyNE";
    QJsonObject jObj;
    jObj.insert("email",QJsonValue::fromVariant(uname));
    jObj.insert("password", QJsonValue::fromVariant(password));
    jObj.insert("returnSecureToken", QJsonValue::fromVariant(true));
    QJsonDocument doc(jObj);
    login_httpWorker->post(URL,doc.toJson());
}



void Firebase::loginResults(QVariantMap response)
{

    idToken = response["idToken"].toString();
    refreshToken = response["refreshToken"].toString();

    qInfo() << "in Firebase::loginResults";
    qInfo() << "LOGIN_RESULTS: ID_TOKEN: " << idToken;
    qInfo() << "LOGIN_RESULTS: REFRESH_TOKEN: " << refreshToken;

    bool success = idToken != "" && refreshToken != "";
    qInfo() << "LOGIN_RESULTS: SUCCESS: " << success;
    if (success){
        refreshAuth();
    }

    //delete login_httpWorker;
}


void Firebase::refreshAuth()
{
    try {
        QString URL = "https://securetoken.googleapis.com/v1/token?key=AIzaSyDOuF3bf6XnwzKZZoYX7tT_nV4JEI8IyNE";
        QJsonObject jObj;
        jObj.insert("grant_type",QJsonValue::fromVariant("refresh_token"));
        jObj.insert("refresh_token", QJsonValue::fromVariant(refreshToken));
        QJsonDocument doc(jObj);
        refreshLogin_httpWorker->post(URL,doc.toJson());

    }catch (...) {
        qDebug() << "Error occured in refreshAuth";
    }

}

void Firebase::refreshAuthResults(QVariantMap response)
{

    try {
        idToken = response["id_token"].toString();
        refreshToken = response["refresh_token"].toString();
        userId = response["user_id"].toString();
        expiresIn = response["expires_in"].toInt();

        qInfo() << "in Firebase::refreshAuthResults";
        qInfo() << "REFRESH_AUTH_RESHULTS: ID_TOKEN: " << idToken;
        qInfo() << "REFRESH_AUTH_RESHULTS: REFRESH_TOKEN: " << refreshToken;
        qInfo() << "REFRESH_AUTH_RESHULTS: EXPIRES_IN: " << expiresIn;
        qInfo() << "REFRESH_AUTH_RESULTS: USER_ID: " << userId;

        bool success = idToken != "" && refreshToken != "" && userId != "" && expiresIn != 0;
        qInfo() << "REFRESH_AUTH_RESULTS: SUCCESS: " << success;
        if (success){
            qInfo() << "REFRESH_AUTH_RESULTS: SETTING INTERVAL";
//            refreshAuthTimer.setInterval(10000);
//            refreshAuthTimer.setSingleShot(true);
//            connect(&refreshAuthTimer, &QTimer::timeout, this, &Firebase::refreshAuth);
//            refreshAuthTimer.start();
            QTimer::singleShot(10000, this, &Firebase::refreshAuth);
        }

        //delete refreshLogin_httpWorker;
    } catch (...) {
        qInfo() << "error occured in refreshAuthResults";
    }

}

void Firebase::getSetTemps()
{
    QString URL = "https://firestore.googleapis.com/v1/projects/zagermonitoringfb/databases/default/documents/";

}




