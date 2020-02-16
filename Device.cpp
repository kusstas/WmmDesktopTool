#include "Device.h"

#include <QEventLoop>
#include <QTimer>

Device::Device(QObject* parent)
    : QObject(parent)
{
    m_serial.setBaudRate(QSerialPort::Baud115200);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    m_serial.setReadBufferSize(1048576);

    connect(&m_serial, &QSerialPort::aboutToClose, this, &Device::onAboutToClose);
    connect(&m_serial, &QSerialPort::readyRead, this, &Device::onReadyRead);
}

bool Device::connected() const
{
    return m_connected;
}

QString Device::portName() const
{
    return m_portName;
}

SettingsQmlProxy* Device::settings() const
{
    return m_settings;
}

UserPreferencesQmlProxy* Device::userPreferences() const
{
    return m_userPreferences;
}

QDateTime Device::dateTime() const
{
    return m_dateTime;
}

bool Device::open()
{
    if (m_tryConnect || m_connected)
    {
        return false;
    }

    m_tryConnect = true;
    m_serial.setPortName(portName());
    bool success = m_serial.open(QSerialPort::ReadWrite);

    if (success)
    {
        success = readAll();

        if (!success)
        {
            m_serial.close();
        }
    }

    setConnected(success);
    m_tryConnect = false;

    return success;
}

bool Device::close()
{
    m_serial.close();

    return true;
}

void Device::clear()
{
    m_serial.clear();
    setDateTime({});
    setSettings(nullptr);
    setUserPreferences(nullptr);
}

bool Device::readAll()
{
    if (!readDateTime())
    {
        return false;
    }
    if (!readSettings())
    {
        return false;
    }
    if (!readUserPreferences())
    {
        return false;
    }

    return true;
}

bool Device::readSettings()
{
    if (!sendCommand(Command::GetSettings))
    {
        return false;
    }

    auto response = waitResponse();

    if (response.size() != sizeof(Settings))
    {
        return false;
    }

    auto const raw = reinterpret_cast<Settings const*>(response.data());
    setSettings(new SettingsQmlProxy(*raw, this));

    return true;
}

bool Device::readUserPreferences()
{
    if (!sendCommand(Command::GetUserPreferences))
    {
        return false;
    }

    auto response = waitResponse();

    if (response.size() != sizeof(SerializedData))
    {
        return false;
    }

    auto const raw = reinterpret_cast<SerializedData const*>(response.data());
    setUserPreferences(new UserPreferencesQmlProxy(*raw, this));

    return true;
}

bool Device::readDateTime()
{
    if (!sendCommand(Command::GetDateTime))
    {
        return false;
    }

    auto response = waitResponse();

    if (response.size() != sizeof(DateTime))
    {
        return false;
    }

    auto const raw = reinterpret_cast<DateTime const*>(response.data());

    QDateTime tmp;

    tmp.setDate(QDate(raw->date.year + YEAR_SHIFT, raw->date.month, raw->date.day));
    tmp.setTime(QTime(raw->time.hour, raw->time.minutes, raw->time.seconds));

    setDateTime(tmp);

    return true;
}

bool Device::submitAll()
{
    if (!submitDataTime())
    {
        return false;
    }
    if (!submitSettings())
    {
        return false;
    }
    if (!submitUserPreferences())
    {
        return false;
    }

    return true;
}

bool Device::submitSettings()
{
    if (!settings())
    {
        return false;
    }

    auto const raw = settings()->toDeviceData();

    QByteArray data(reinterpret_cast<char const*>(&raw), sizeof(raw));

    return sendCommand(Command::SetSettings, data);
}

bool Device::submitUserPreferences()
{
    if (!userPreferences())
    {
        return false;
    }

    auto const raw = userPreferences()->toDeviceData();

    QByteArray data(reinterpret_cast<char const*>(&raw), sizeof(raw));

    return sendCommand(Command::SetUserPreferences, data);
}

bool Device::submitDataTime()
{
    if (!dateTime().isValid())
    {
        return false;
    }

    DateTime raw;
    QDate date = dateTime().date();
    QTime time = dateTime().time();

    raw.date.year = date.year();
    raw.date.month = date.month();
    raw.date.day = date.day();
    raw.date.week = date.dayOfWeek();

    raw.time.hour = time.hour();
    raw.time.minutes = time.minute();
    raw.time.seconds =  time.second();

    QByteArray data(reinterpret_cast<char const*>(&raw), sizeof(raw));

    return sendCommand(Command::SetDateTime, data);
}

bool Device::resetSettings()
{
    return sendCommand(Command::ResetSettings);
}

bool Device::resetUserPreferences()
{
    return sendCommand(Command::ResetUserPreferences);
}

void Device::setPortName(QString portName)
{
    if (m_portName == portName)
        return;

    m_portName = portName;
    emit portNameChanged(m_portName);
}

void Device::setDateTime(QDateTime dateTime)
{
    if (m_dateTime == dateTime)
        return;

    m_dateTime = dateTime;
    emit dateTimeChanged(m_dateTime);
}

void Device::setConnected(bool connected)
{
    if (m_connected == connected)
        return;

    m_connected = connected;
    emit connectedChanged(m_connected);
}

void Device::setSettings(SettingsQmlProxy* settings)
{
    if (m_settings == settings)
        return;

    auto const previous = m_settings;

    m_settings = settings;
    emit settingsChanged(m_settings);

    if (previous)
    {
        previous->deleteLater();
    }
}

void Device::setUserPreferences(UserPreferencesQmlProxy* userPreferences)
{
    if (m_userPreferences == userPreferences)
        return;

    auto const previous = m_userPreferences;

    m_userPreferences = userPreferences;
    emit userPreferencesChanged(m_userPreferences);

    if (previous)
    {
        previous->deleteLater();
    }
}

void Device::onAboutToClose()
{
    setConnected(false);
    clear();
}

void Device::onReadyRead()
{
    if (!m_waitResponse)
    {
        return;
    }

    auto tmp = m_serial.readAll();

    int i = 0;
    while (i < tmp.size())
    {
        if (m_targetSizeReceive < 0)
        {
            auto size = static_cast<int>(tmp[i++]);

            if (size > 0)
            {
                m_targetSizeReceive = size;
                m_responseBuffer.clear();
            }
            else
            {
                emit responseError();
                return;
            }
        }
        else
        {
            m_responseBuffer.append(tmp.data() + i,  tmp.size() - i);
            i = tmp.size();
        }

        if (m_responseBuffer.size() == m_targetSizeReceive)
        {
            emit responseComplete();
            return;
        }
    }
}

bool Device::sendCommand(Command command, QByteArray data)
{
    char const cmdRaw[] = {static_cast<char>(command),
                           static_cast<char>(data.size())};
    QByteArray const cmd{cmdRaw, sizeof(cmdRaw)};

    if (m_serial.write(cmd) != cmd.size())
    {
        return false;
    }
    if (!data.isEmpty())
    {
        if (m_serial.write(data) != data.size())
        {
            return false;
        }
    }

    m_serial.flush();

    return true;
}

void Device::resetReceive()
{
    m_targetSizeReceive = -1;
    m_responseBuffer.clear();
}

QByteArray Device::waitResponse(int timeout)
{
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(this, &Device::responseComplete, &loop, &QEventLoop::quit);
    connect(this, &Device::responseError, &loop, &QEventLoop::quit);

    m_waitResponse = true;
    timer.start();
    loop.exec();
    timer.stop();
    m_waitResponse = false;

    auto tmp = m_responseBuffer;
    resetReceive();

    return tmp;
}
