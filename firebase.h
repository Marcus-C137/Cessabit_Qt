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
#include "wifimanager.h"
#include "localdb.h"
#include "arduinocoms.h"

class Firebase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool signedIn READ signedIn WRITE setSignedIn NOTIFY signedInChanged)
    Q_PROPERTY(QString uname READ uname);
    Q_PROPERTY(bool almSetting_deviceOffline        READ almSetting_deviceOffline       WRITE setAlmSetting_deviceOffline           NOTIFY almSetting_deviceOfflineChanged)
    Q_PROPERTY(bool almSetting_lowTemp              READ almSetting_lowTemp             WRITE setAlmSetting_lowTemp                 NOTIFY almSetting_lowTempChanged)
    Q_PROPERTY(bool almSetting_highTemp             READ almSetting_highTemp            WRITE setAlmSetting_highTemp                NOTIFY almSetting_highTempChanged)
    Q_PROPERTY(bool almSetting_notResponsive        READ almSetting_notResponsive       WRITE setAlmSetting_notResponsive           NOTIFY almSetting_notResponsiveChanged)
    Q_PROPERTY(bool almSetting_sensorDisconnect     READ almSetting_sensorDisconnect    WRITE setAlmSetting_sensorDisconnect        NOTIFY almSetting_sensorDisonncetChanged)
    Q_PROPERTY(bool notifSetting_pushNotifications  READ notifSetting_pushNotifications WRITE setnotifSetting_pushNotifications     NOTIFY notifSetting_pushNotificationsChanged)
    Q_PROPERTY(bool notifSetting_textMe             READ notifSetting_textMe            WRITE setnotifSetting_textMe                NOTIFY notifSetting_textMeChanged)
    Q_PROPERTY(bool notifSetting_textFriends        READ notifSetting_textFriends       WRITE setnotifSetting_textFriends           NOTIFY notifSetting_textFriendsChanged)

public:
    ~Firebase();
    explicit Firebase(QObject *parent = nullptr);
    QVector<qreal> db_lowTemps;
    QVector<qreal> db_highTemps;
    QVector<qreal> db_setTemps;
    static void doDeleteLater(HttpsWorker * obj);
    void setWifiManager(WifiManager *wifiRef);
    void setArduinoComs(ArduinoComs *comsRef);
    void postCurrentTemps();
    bool signedIn() const{return m_signedIn;}
    bool almSetting_deviceOffline() const{return m_almSetting_deviceOffline;}
    bool almSetting_highTemp() const{return m_almSetting_highTemp;}
    bool almSetting_lowTemp() const{return m_almSetting_lowTemp;}
    bool almSetting_notResponsive() const{return m_almSetting_notResponsive;}
    bool almSetting_sensorDisconnect() const{return m_almSetting_sensorDisconnect;}
    bool notifSetting_pushNotifications() const{return m_notifSetting_pushNotifications;}
    bool notifSetting_textMe() const{return m_notifSetting_textMe;}
    bool notifSetting_textFriends() const{return m_notifSetting_textFriends;}
    QString uname() const{return m_uname;}

signals:
    void signedInChanged(bool signedIn);
    void signInResults(bool successful);
    void refreshedAuth();
    void almSetting_highTempChanged(bool almSetting_highTemp);
    void almSetting_deviceOfflineChanged(bool almSetting_deviceOffline);
    void almSetting_lowTempChanged(bool almSetting_lowTemp);
    void almSetting_notResponsiveChanged(bool almSetting_notResponsive);
    void almSetting_sensorDisonncetChanged(bool almSetting_sensorDisconnect);
    void notifSetting_pushNotificationsChanged(bool notifSetting_pushNotifications);
    void notifSetting_textMeChanged(bool notifSetting_textMe);
    void notifSetting_textFriendsChanged(bool notifSetting_textFriends);

public slots:
    void loginViaDB();
    void linkAccount(QString uname, QString password);
    void updateSettings(int setting, bool val);
    void refreshAuth();
    void refreshAuthResults(QVariantMap response);
    void addDevice();
    void addDeviceResults(QVariantMap response);
    void updateSettingsDoc(QString category, QString field, QVariant val);
    void updateSettingsDocResults(QVariantMap map);
    void updateTempsDoc(QDateTime time, QVariantList tempsAndPowers);
    void updateTempsDocResults(QVariantMap map);
    void updateAlmsDoc(int almCategory);
    void updateAlmsDocResults(QVariantMap map);
    void readDoc();
    void readDocResults(QVariantMap response);
    void login(QString uname, QString password);
    void loginResults(QVariantMap response);
    void addUserInfo(QString uname, QString password);
    void newTempandPowerReading(QVariantList tempsAndPowers);
    void setSignedIn(bool signedIn)
    {
        if (m_signedIn == signedIn)return;
        m_signedIn = signedIn;
        emit signedInChanged(m_signedIn);
    }

    void setAlmSetting_highTemp(bool almSetting_highTemp)
    {
        if (m_almSetting_highTemp == almSetting_highTemp)return;
        m_almSetting_highTemp = almSetting_highTemp;
        emit almSetting_highTempChanged(m_almSetting_highTemp);
    }

    void setAlmSetting_deviceOffline(bool almSetting_deviceOffline)
    {
        if (m_almSetting_deviceOffline == almSetting_deviceOffline) return;
        m_almSetting_deviceOffline = almSetting_deviceOffline;
        emit almSetting_deviceOfflineChanged(m_almSetting_deviceOffline);
    }

    void setAlmSetting_lowTemp(bool almSetting_lowTemp)
    {
        if (m_almSetting_lowTemp == almSetting_lowTemp) return;
        m_almSetting_lowTemp = almSetting_lowTemp;
        emit almSetting_lowTempChanged(m_almSetting_lowTemp);
    }

    void setAlmSetting_notResponsive(bool almSetting_notResponsive)
    {
        if (m_almSetting_notResponsive == almSetting_notResponsive) return;
        m_almSetting_notResponsive = almSetting_notResponsive;
        emit almSetting_notResponsiveChanged(m_almSetting_notResponsive);
    }

    void setAlmSetting_sensorDisconnect(bool almSetting_sensorDisconnect)
    {
        if (m_almSetting_sensorDisconnect == almSetting_sensorDisconnect) return;
        m_almSetting_sensorDisconnect = almSetting_sensorDisconnect;
        emit almSetting_sensorDisonncetChanged(m_almSetting_sensorDisconnect);
    }

    void setnotifSetting_pushNotifications(bool notifSetting_pushNotifications)
    {
        if (m_notifSetting_pushNotifications == notifSetting_pushNotifications) return;
        m_notifSetting_pushNotifications = notifSetting_pushNotifications;
        emit notifSetting_pushNotificationsChanged(m_notifSetting_pushNotifications);
    }

    void setnotifSetting_textMe(bool notifSetting_textMe)
    {
        if (m_notifSetting_textMe == notifSetting_textMe) return;
        m_notifSetting_textMe = notifSetting_textMe;
        emit notifSetting_textMeChanged(m_notifSetting_textMe);
    }

    void setnotifSetting_textFriends(bool notifSetting_textFriends)
    {
        if (m_notifSetting_textFriends == notifSetting_textFriends) return;
        m_notifSetting_textFriends = notifSetting_textFriends;
        emit notifSetting_textFriendsChanged(m_notifSetting_textFriends);
    }

private slots:

private:
    ArduinoComs *coms;
    WifiManager *wifiManager;
    HttpsWorker *login_httpWorker;
    HttpsWorker *readDoc_httpWorker;
    HttpsWorker *refreshLogin_httpWorker;
    HttpsWorker *addDevice_httpWorker;
    HttpsWorker *updateSettingsDoc_httpWorker;
    HttpsWorker *updateTempsDoc_httpWorker;
    HttpsWorker *updateAlmsDoc_httpWorker;
    QTimer *downloadDocTimer;
    QTimer refreshAuthTimer;
    QString idToken;
    QString accessToken;
    QString refreshToken;
    QString userId;
    QString m_uname;
    QString m_password;
    int expiresIn;
    bool m_signedIn;
    bool m_almSetting_highTemp;
    bool m_almSetting_lowTemp;
    bool m_almSetting_deviceOffline;
    bool m_almSetting_notResponsive;
    bool m_almSetting_sensorDisconnect;
    bool m_notifSetting_pushNotifications;
    bool m_notifSetting_textFriends;
    bool m_notifSetting_textMe;
    bool chngRemote_temps;
    bool chngRemote_notifs;
    bool chngRemote_alms;
    bool initialDBdownloaded;

};

#endif // FIREBASE_H
