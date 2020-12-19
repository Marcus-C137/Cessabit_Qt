#include "firebase.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <QSharedPointer>
#include "firebase.h"
#include "firebase.h"
#include "httpsworker.h"
#include "localdb.h"
#include "singleton.h"
#include "wifimanager.h"



Firebase::Firebase(QObject *parent) : QObject(parent)
{
    idToken = "";
    refreshToken = "";
    userId = "";
    expiresIn = 0;
    m_signedIn                      = false;
    initialDBdownloaded             = false;
    m_almSetting_highTemp           = false;
    m_almSetting_lowTemp            = false;
    m_almSetting_deviceOffline      = false;
    m_almSetting_notResponsive      = false;
    m_almSetting_sensorDisconnect   = false;
    m_notifSetting_pushNotifications= false;
    m_notifSetting_textFriends      = false;
    m_notifSetting_textMe           = false;
    chngRemote_temps                = false;
    chngRemote_notifs               = false;
    chngRemote_alms                 = false;
    db_lowTemps                     = {0.0,0.0,0.0,0.0};
    db_highTemps                    = {0.0,0.0,0.0,0.0};
    db_setTemps                     = {0.0,0.0,0.0,0.0};
    addDevice_httpWorker            = new HttpsWorker();
    login_httpWorker                = new HttpsWorker();
    refreshLogin_httpWorker         = new HttpsWorker();
    readDoc_httpWorker              = new HttpsWorker();
    updateSettingsDoc_httpWorker    = new HttpsWorker();
    updateTempsDoc_httpWorker       = new HttpsWorker();
    updateAlmsDoc_httpWorker        = new HttpsWorker();
    downloadDocTimer                = new QTimer(this);
    connect(login_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(loginResults(QVariantMap)));
    connect(refreshLogin_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(refreshAuthResults(QVariantMap)));
    connect(readDoc_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(readDocResults(QVariantMap)));
    connect(addDevice_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(addDeviceResults(QVariantMap)));
    connect(updateTempsDoc_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(updateTempsDocResults(QVariantMap)));
    connect(updateSettingsDoc_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(updateSettingsDocResults(QVariantMap)));
    connect(updateAlmsDoc_httpWorker, SIGNAL(requestFinished(QVariantMap)), this, SLOT(updateAlmsDocResults(QVariantMap)));
    connect(downloadDocTimer, SIGNAL(timeout()), this, SLOT(readDoc()));
    connect(&Singleton<Localdb>::GetInstance(), SIGNAL(newAlarmTempFirebase(int)), this, SLOT(updateAlmsDoc(int)));
    downloadDocTimer->start(10000);
}

Firebase::~Firebase()
{
    qInfo() << "firebase destroyed";
}

void Firebase::setWifiManager(WifiManager *wifiRef)
{
    wifiManager = wifiRef;
}

void Firebase::setArduinoComs(ArduinoComs *comsRef)
{
    coms = comsRef;
    connect(coms, SIGNAL(firebaseUpdate(QVariantList)), this, SLOT(newTempandPowerReading(QVariantList)));
}

void Firebase::login(QString uname, QString password)
{
    //qInfo() << "uname: " << uname;
    //qInfo() << "password: " << password;
    QString URL = "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=AIzaSyDOuF3bf6XnwzKZZoYX7tT_nV4JEI8IyNE";
    QVariantMap params;
    QVariantMap header;
    QVariantMap body;
    body["email"] = uname;
    body["password"] = password;
    body["returnSecureToken"]= true;
    login_httpWorker->post(URL,params, header, body);

}

void Firebase::addUserInfo(QString uname, QString password){
      qInfo() << "Firebase: adduserInfo() " << uname << " " << password;
      Singleton<Localdb>::GetInstance().addUserInfo(uname,password);
}


void Firebase::loginViaDB()
{
   QStringList accountInfo = Singleton<Localdb>::GetInstance().checkUserAccount();
   QString uname = accountInfo[0];
   QString password = accountInfo[1];
   qInfo() << uname;
   qInfo() << password;
   if(uname.length() == 0 || password.length() == 0){
       qWarning() << Q_FUNC_INFO << "uname and/or password are null in DB";
       emit setSignedIn(false);
       return;
   }
   this->login(uname, password);
}


void Firebase::loginResults(QVariantMap response)
{
    idToken = response["idToken"].toString();
    refreshToken = response["refreshToken"].toString();
    QString user = response["email"].toString();

    qInfo() << "in Firebase::loginResults";
    //qInfo() << "LOGIN_RESULTS: ID_TOKEN: " << idToken;
    //qInfo() << "LOGIN_RESULTS: REFRESH_TOKEN: " << refreshToken;

    bool successful = idToken != "" && refreshToken != "";
    qInfo() << "LOGIN_RESULTS: SUCCESS: " << successful;
    if (successful){
        refreshAuth();
        m_signedIn = true;
        m_uname = user;
        emit signInResults(successful);

    }else{
        m_signedIn = false;
        emit signInResults(successful);
    }
}


void Firebase::refreshAuth()
{
    try {
        QString URL = "https://securetoken.googleapis.com/v1/token?key=AIzaSyDOuF3bf6XnwzKZZoYX7tT_nV4JEI8IyNE";
        QJsonObject jObj;
        QVariantMap params;
        QVariantMap header;
        QVariantMap body;
        body["grant_type"] = "refresh_token";
        body["refresh_token"] = refreshToken;
        refreshLogin_httpWorker->post(URL, params, header, body);

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
        accessToken = response["access_token"].toString();
        expiresIn = response["expires_in"].toInt();

        qInfo() << "in Firebase::refreshAuthResults";
        //qInfo() << "REFRESH_AUTH_RESHULTS: ID_TOKEN: " << idToken;
        //qInfo() << "REFRESH_AUTH_RESHULTS: REFRESH_TOKEN: " << refreshToken;
        //qInfo() << "REFRESH_AUTH_RESHULTS: EXPIRES_IN: " << expiresIn;
        //qInfo() << "REFRESH_AUTH_RESULTS: USER_ID: " << userId;

        bool success = idToken != "" && refreshToken != "" && userId != "" && expiresIn != 0;
        qInfo() << "REFRESH_AUTH_RESULTS: SUCCESS: " << success;
        if (success){
            qInfo() << "REFRESH_AUTH_RESULTS: SETTING INTERVAL";
            QTimer::singleShot((expiresIn-10)*1000, this, &Firebase::refreshAuth);
            emit refreshedAuth();
        }

    } catch (...) {
        qInfo() << "error occured in refreshAuthResults";
    }

}

void Firebase::updateSettings(int setting, bool val)
{
    switch (setting){
    case 1: // deviceOffline
        updateSettingsDoc("alarms", "deviceOffline", val);
        break;
    case 2: // lowTemp
        updateSettingsDoc("alarms", "lowTemp", val);
        break;
    case 3: // highTemp
        updateSettingsDoc("alarms", "highTemp", val);
        break;
    case 4: // notResponsive
        updateSettingsDoc("alarms", "notResponsive", val);
        break;
    case 5: // sensorDisconnect
        updateSettingsDoc("alarms", "sensorDisconnect", val);
        break;
    case 6: // pushNotifications
        updateSettingsDoc("notifications", "pushNotifications", val);
        break;
    case 7: // text Me
        updateSettingsDoc("notifications", "textMe", val);
        break;
    case 8: // highTemp
        updateSettingsDoc("notifications", "textFriends", val);
        break;
    }
}

void Firebase::linkAccount(QString uname, QString password)
{
    m_uname = uname;
    m_password = password;
    this->login(uname, password);
    connect(this, SIGNAL(refreshedAuth()), this, SLOT(addDevice())); // after logging in wait for auth token to be able to write to db
}

void Firebase::addDevice(){
    disconnect(this, SIGNAL(refreshedAuth()), this, SLOT(addDevice()));
    qInfo() << Q_FUNC_INFO;
    if(m_signedIn){
        this->addUserInfo(m_uname, m_password);
        QString URL = "https://us-central1-zagermonitoringfb.cloudfunctions.net/addDevice";
        QVariantMap params;
        QVariantMap header;
        QVariantMap body;
        header["Authorization"] = "Bearer " + idToken;
        body["macID"] = wifiManager->macID;
        QJsonObject jObj;
        QString id = wifiManager->macID;
        qInfo() << Q_FUNC_INFO << "id" << id;
        jObj.insert("macID",id);
        QJsonDocument doc(jObj);

    }else{
        qDebug() << Q_FUNC_INFO << "Trying to add device, but not signed into account";
    }
}


void Firebase::addDeviceResults(QVariantMap response)
{
    qInfo() << Q_FUNC_INFO << response;
}

void Firebase::readDoc()
{
    if (m_signedIn){
        QString docName = wifiManager->macID;
        QString URL = "https://firestore.googleapis.com/v1/projects/zagermonitoringfb/databases/(default)/documents/users/" + userId + "/devices/" + docName;
        QVariantMap params;
        QVariantMap header;
        QVariantMap body;
        header["Authorization"] = "Bearer " + accessToken;
        readDoc_httpWorker->get(URL, params, header, body);
    }
}

void Firebase::readDocResults(QVariantMap response)
{
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(response);
    QJsonValue jsonVal = jsonDoc["fields"]["deviceOffline"]["booleanValue"];
    chngRemote_alms = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["changedRemotely"]["booleanValue"].toBool();
    chngRemote_notifs = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["changedRemotely"]["booleanValue"].toBool();
    chngRemote_temps = jsonDoc["fields"]["setAlarms"]["mapValue"]["fields"]["changedRemotely"]["booleanValue"].toBool();
    bool almSetting_highTemp             = false;
    bool almSetting_lowTemp              = false;
    bool almSetting_deviceOffline        = false;
    bool almSetting_notResponsive        = false;
    bool almSetting_sensorDisconnect     = false;
    bool notifSetting_pushNotifications  = false;
    bool notifSetting_textFriends        = false;
    bool notifSetting_textMe             = false;
    if (!initialDBdownloaded){
        initialDBdownloaded = true;
        almSetting_highTemp             = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["highTemp"]["booleanValue"].toBool();
        almSetting_lowTemp              = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["lowTemp"]["booleanValue"].toBool();
        almSetting_deviceOffline        = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["deviceOffline"]["booleanValue"].toBool();
        almSetting_notResponsive        = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["notResponsive"]["booleanValue"].toBool();
        almSetting_sensorDisconnect     = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["sensorDisconnect"]["booleanValue"].toBool();
        notifSetting_pushNotifications  = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["pushNotifications"]["booleanValue"].toBool();
        notifSetting_textFriends        = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["textFriends"]["booleanValue"].toBool();
        notifSetting_textMe             = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["textMe"]["booleanValue"].toBool();
    }
    if(chngRemote_alms){
        almSetting_highTemp             = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["highTemp"]["booleanValue"].toBool();
        almSetting_lowTemp              = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["lowTemp"]["booleanValue"].toBool();
        almSetting_deviceOffline        = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["deviceOffline"]["booleanValue"].toBool();
        almSetting_notResponsive        = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["notResponsive"]["booleanValue"].toBool();
        almSetting_sensorDisconnect     = jsonDoc["fields"]["alarms"]["mapValue"]["fields"]["sensorDisconnect"]["booleanValue"].toBool();
    }
    if(chngRemote_notifs){
        notifSetting_pushNotifications  = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["pushNotifications"]["booleanValue"].toBool();
        notifSetting_textFriends        = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["textFriends"]["booleanValue"].toBool();
        notifSetting_textMe             = jsonDoc["fields"]["notifications"]["mapValue"]["fields"]["textMe"]["booleanValue"].toBool();
    }
    if(chngRemote_temps){

    }
    setAlmSetting_deviceOffline(almSetting_deviceOffline);
    setAlmSetting_lowTemp(almSetting_lowTemp);
    setAlmSetting_highTemp(almSetting_highTemp);
    setAlmSetting_notResponsive(almSetting_notResponsive);
    setAlmSetting_sensorDisconnect(almSetting_sensorDisconnect);
    setnotifSetting_pushNotifications(notifSetting_pushNotifications);
    setnotifSetting_textMe(notifSetting_textMe);
    setnotifSetting_textFriends(notifSetting_textFriends);
}


void Firebase::updateSettingsDoc(QString category, QString field, QVariant val)
{
    if(m_signedIn){
        QString docName = wifiManager->macID;
        QString URL = "https://firestore.googleapis.com/v1/projects/zagermonitoringfb/databases/(default)/documents/users/" + userId + "/devices/" + docName;
        QVariantMap params;
        QVariantMap header;
        QVariantMap body_val;
        QVariantMap body_field;
        QVariantMap body_fields;
        QVariantMap body_mv;
        QVariantMap body_cat;
        QVariantMap body;
        params["updateMask.fieldPaths"] = category + "." + field;
        header["Authorization"] = "Bearer " + accessToken;
        body_val["booleanValue"] = val;
        body_field[field]= body_val;
        body_fields["fields"] = body_field;
        body_mv["mapValue"] = body_fields;
        body_cat[category] = body_mv;
        body["fields"] = body_cat;
        updateSettingsDoc_httpWorker->patch(URL, params, header, body);
    }else{
        qDebug() << Q_FUNC_INFO << "Not signed in but trying to change settings in Firebase DB";
    }
}

void Firebase::updateSettingsDocResults(QVariantMap map)
{

}

void Firebase::newTempandPowerReading(QVariantList tempsAndPowers)
{
    QDateTime time = QDateTime(QDateTime::currentDateTime());
    updateTempsDoc(time, tempsAndPowers);
}

void Firebase::updateTempsDoc(QDateTime time, QVariantList tempsAndPowers){
    QString docName = wifiManager->macID;
    QDate date = time.date();
    QString time_S = QString::number(time.currentSecsSinceEpoch());
    int hourEpoch = qFloor(time.currentSecsSinceEpoch()/3600);
    QString hourEpoch_S = QString::number(hourEpoch);
    QString URL = "https://firestore.googleapis.com/v1/projects/zagermonitoringfb/databases/(default)/documents/users/" + userId + "/devices/" + docName +"/Temperatures/" + date.toString(Qt::ISODate);
    QVariantMap params;
    QVariantMap header;
    QVariantMap body_temp1;
    QVariantMap body_temp2;
    QVariantMap body_temp3;
    QVariantMap body_temp4;
    QVariantList body_temp_list;
    QVariantMap body_power1;
    QVariantMap body_power2;
    QVariantMap body_power3;
    QVariantMap body_power4;
    QVariantList body_power_list;
    QVariantMap body_t_values;
    QVariantMap body_t_arrayValue;
    QVariantMap body_p_values;
    QVariantMap body_p_arrayValue;
    QVariantMap body_temps_powers;
    QVariantMap body_fields;
    QVariantMap body_mv;
    QVariantMap body_time;
    QVariantMap body_fields1;
    QVariantMap body_mv1;
    QVariantMap body_time_wrap;
    QVariantMap body;
    params["updateMask.fieldPaths"] = "temps.T" + time_S;
    params["mask.fieldPaths"] = "temps.T" + time_S;
    header["Authorization"] = "Bearer " + accessToken;
    body_temp1["doubleValue"] = tempsAndPowers[0];
    body_temp2["doubleValue"] = tempsAndPowers[1];
    body_temp3["doubleValue"] = tempsAndPowers[2];
    body_temp4["doubleValue"] = tempsAndPowers[3];
    body_temp_list = {body_temp1, body_temp2, body_temp3, body_temp4};
    body_power1["doubleValue"] = tempsAndPowers[4];
    body_power2["doubleValue"] = tempsAndPowers[5];
    body_power3["doubleValue"] = tempsAndPowers[6];
    body_power4["doubleValue"] = tempsAndPowers[7];
    body_power_list = {body_power1, body_power2, body_power3, body_power4};
    body_t_values["values"] = body_temp_list;
    body_t_arrayValue["arrayValue"] = body_t_values;
    body_temps_powers["Temps"] = body_t_arrayValue;
    body_p_values["values"] = body_power_list;
    body_p_arrayValue["arrayValue"] = body_p_values;
    body_temps_powers["Powers"] = body_p_arrayValue;
    body_fields["fields"] = body_temps_powers;
    body_mv["mapValue"] = body_fields;
    body_time["T"+time_S] = body_mv;
    body_fields1["fields"] = body_time;
    body_mv1["mapValue"] = body_fields1;
    body_time_wrap["temps"] = body_mv1;
    body["fields"] = body_time_wrap;
    updateTempsDoc_httpWorker->patch(URL, params, header, body);
}


void Firebase::updateTempsDocResults(QVariantMap map)
{

}

void Firebase::updateAlmsDoc(int almCategory){
    QString docName = wifiManager->macID;
    QString almLabel;
    QString URL = "https://firestore.googleapis.com/v1/projects/zagermonitoringfb/databases/(default)/documents/users/" + userId + "/devices/" + docName;
    QVariantMap params;
    QVariantMap header;
    QVariantMap body_temp1;
    QVariantMap body_temp2;
    QVariantMap body_temp3;
    QVariantMap body_temp4;
    QVariantList body_temp_list;
    QVariantMap body_values;
    QVariantMap body_arrayValue;
    QVariantMap body_temps;
    QVariantMap body_fields;
    QVariantMap body_mv;
    QVariantMap body_time;
    QVariantMap body;

    if(almCategory == 0){
        almLabel = "setTemperatures";
    }else if (almCategory == 1){
        almLabel = "highAlarms";
    }else if (almCategory == 2){
        almLabel = "lowAlarms";
    }

    params["updateMask.fieldPaths"] = "setAlarms." + almLabel;
    header["Authorization"] = "Bearer " + accessToken;
    body_temp1["doubleValue"] = Singleton<Localdb>::GetInstance().setAlms[0][almCategory];
    body_temp2["doubleValue"] = Singleton<Localdb>::GetInstance().setAlms[1][almCategory];
    body_temp3["doubleValue"] = Singleton<Localdb>::GetInstance().setAlms[2][almCategory];
    body_temp4["doubleValue"] = Singleton<Localdb>::GetInstance().setAlms[3][almCategory];
    body_temp_list = {body_temp1, body_temp2, body_temp3, body_temp4};
    body_values["values"] = body_temp_list;
    body_arrayValue["arrayValue"] = body_values;
    body_temps[almLabel] = body_arrayValue;
    body_fields["fields"] = body_temps;
    body_mv["mapValue"] = body_fields;
    body_time["setAlarms"] = body_mv;
    body["fields"] = body_time;
    updateAlmsDoc_httpWorker->patch(URL, params, header, body);
}

void Firebase::updateAlmsDocResults(QVariantMap map)
{

}

