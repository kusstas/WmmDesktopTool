#pragma once

#include <QObject>
#include <QSerialPort>
#include <QDateTime>

#include "DeviceQmlProxies.h"

class Device : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)

    Q_PROPERTY(SettingsQmlProxy* settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(UserPreferencesQmlProxy* userPreferences READ userPreferences NOTIFY userPreferencesChanged)
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY dateTimeChanged)

public:
    explicit Device(QObject* parent = nullptr);

    bool connected() const;
    QString portName() const;
    SettingsQmlProxy* settings() const;
    UserPreferencesQmlProxy* userPreferences() const;
    QDateTime dateTime() const;

public slots:
    bool open();
    bool close();
    void clear();

    bool readAll();
    bool readSettings();
    bool readUserPreferences();
    bool readDateTime();

    bool submitAll();
    bool submitSettings();
    bool submitUserPreferences();
    bool submitDataTime();

    bool resetSettings();
    bool resetUserPreferences();

    void setPortName(QString portName);
    void setDateTime(QDateTime dateTime);

private slots:
    void setConnected(bool connected);
    void setSettings(SettingsQmlProxy* settings);
    void setUserPreferences(UserPreferencesQmlProxy* userPreferences);
    void onAboutToClose();
    void onReadyRead();

signals:
    void connectedChanged(bool connected);
    void portNameChanged(QString portName);
    void settingsChanged(SettingsQmlProxy* settings);
    void userPreferencesChanged(UserPreferencesQmlProxy* userPreferences);
    void dateTimeChanged(QDateTime dateTime);

    void responseComplete();
    void responseError();

private:
    bool sendCommand(Command command, QByteArray data = {});
    void resetReceive();

    QByteArray waitResponse(int timeout = 1000);

private:
    bool m_connected = false;
    bool m_tryConnect = false;
    QString m_portName = "";
    SettingsQmlProxy* m_settings = nullptr;
    UserPreferencesQmlProxy* m_userPreferences= nullptr;

    QSerialPort m_serial{};

    bool m_waitResponse = false;
    int m_targetSizeReceive = -1;
    QByteArray m_responseBuffer{};
    QDateTime m_dateTime{};
};
