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
    Q_PROPERTY(bool signedIn                        READ signedIn                       WRITE setSignedIn                           NOTIFY signedInChanged)
    Q_PROPERTY(QString uname                        READ uname);
    Q_PROPERTY(QString version                      READ version                                                                    NOTIFY notifVersion);
    Q_PROPERTY(bool isNewFirmwareAvailable          READ isNewFirmwareAvailable                                                     NOTIFY notif_newFirmwareAvailable);
    Q_PROPERTY(bool notifSetting_pushNotifications  READ notifSetting_pushNotifications WRITE setnotifSetting_pushNotifications     NOTIFY notifSetting_pushNotificationsChanged)
    Q_PROPERTY(bool notifSetting_textMe             READ notifSetting_textMe            WRITE setnotifSetting_textMe                NOTIFY notifSetting_textMeChanged)
    Q_PROPERTY(bool notifSetting_textFriends        READ notifSetting_textFriends       WRITE setnotifSetting_textFriends           NOTIFY notifSetting_textFriendsChanged)

public:
    ~Firebase();
    explicit Firebase(QObject *parent = nullptr);
    Q_INVOKABLE void userRequestedFWDownload();
    QVector<qreal> db_lowTemps;
    QVector<qreal> db_highTemps;
    QVector<qreal> db_setTemps;
    QString uname() const{return m_uname;}
    QString version() const{return m_version;}
    static void doDeleteLater(HttpsWorker * obj);
    void setWifiManager(WifiManager *wifiRef);
    void setArduinoComs(ArduinoComs *comsRef);
    void postCurrentTemps();
    void loadVersionNumber();
    bool signedIn() const{return m_signedIn;}
    bool almSetting_deviceOffline() const{return m_almSetting_deviceOffline;}
    bool almSetting_highTemp() const{return m_almSetting_highTemp;}
    bool almSetting_lowTemp() const{return m_almSetting_lowTemp;}
    bool almSetting_notResponsive() const{return m_almSetting_notResponsive;}
    bool almSetting_sensorDisconnect() const{return m_almSetting_sensorDisconnect;}
    bool notifSetting_pushNotifications() const{return m_notifSetting_pushNotifications;}
    bool notifSetting_textMe() const{return m_notifSetting_textMe;}
    bool notifSetting_textFriends() const{return m_notifSetting_textFriends;}
    bool portsOn[4];
    bool subscribed;
    bool isNewFirmwareAvailable() const{return m_newFirmwareAvailable;}

signals:
    void activateEstop(bool value);
    void signedInChanged(bool signedIn);
    void signInResults(bool successful);
    void subResults(bool subscribed);
    void refreshedAuth();
    void notifVersion(QString version);
    void notifSetting_pushNotificationsChanged(bool notifSetting_pushNotifications);
    void notifSetting_textMeChanged(bool notifSetting_textMe);
    void notifSetting_textFriendsChanged(bool notifSetting_textFriends);
    void notif_newFirmwareAvailable(bool newFirmwareAvailable);

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
    void updatePortsOn(int port, bool portOn);
    void updatePortsOnResults(QVariantMap map);
    void updateGains(int port, int gains);
    void updateGainsResults(QVariantMap map);
    void updateHeartbeatDoc();
    void updateHeartbeatDocResults(QVariantMap map);
    void updateOnlineDocField();
    void updateOnlineDocFieldResults(QVariantMap map);
    void sendAlarm(QString title, QString message);
    void sendAlarmResults(QVariantMap map);
    void downloadNewFirmware(QString URL);
    void downloadNewFirmwareResults(QByteArray file);
    void checkForUpdates();
    void checkForUpdatesResults(QVariantMap map);
    void checkSub();
    void checkSubResults(QVariantMap map);
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
    Process p_chmod;
    Process p_serviceUpdate;
    HttpsWorker *login_httpWorker;
    HttpsWorker *readDoc_httpWorker;
    HttpsWorker *refreshLogin_httpWorker;
    HttpsWorker *addDevice_httpWorker;
    HttpsWorker *updateSettingsDoc_httpWorker;
    HttpsWorker *updateTempsDoc_httpWorker;
    HttpsWorker *updateAlmsDoc_httpWorker;
    HttpsWorker *updateHeartbeatDoc_httpWorker;
    HttpsWorker *updateOnlineDoc_httpWorker;
    HttpsWorker *updatePortsOn_httpWorker;
    HttpsWorker *updateGains_httpWorker;
    HttpsWorker *sendAlarm_httpWorker;
    HttpsWorker *checkForUpdate_httpWorker;
    HttpsWorker *checkSub_httpWorker;
    HttpsWorker *downloadFirm_httpWorker;
    QTimer *downloadDocTimer;
    QTimer *checkForUpdateTimer;
    QTimer refreshAuthTimer;
    QString idToken;
    QString accessToken;
    QString refreshToken;
    QString userId;
    QString m_uname;
    QString m_password;
    QString m_version;
    QString m_downloadURL;
    int expiresIn;
    int latestVersion;
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
    bool m_newFirmwareAvailable;

};

#endif // FIREBASE_H
