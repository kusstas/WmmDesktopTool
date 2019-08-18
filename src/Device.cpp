#include "Device.h"

#include <QDebug>
#include <QDateTime>
#include <QStringList>
#include <QSerialPortInfo>
#include <QTextCodec>
#include <QStringBuilder>

#include <QDir>
#include <QFile>

Device::Device(QObject* parent)
    : QObject(parent)
{
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setStopBits(QSerialPort::OneStop);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
    m_serialPort.setReadBufferSize(1048576);
    connect(&m_serialPort, &QSerialPort::readyRead, this, &Device::onReadyRead);

    m_updatePortsTimer.setInterval(1000);
    m_waitLineTimeoutTimer.setSingleShot(true);
    m_waitLineTimeoutTimer.setInterval(3000);
    m_loadBinaryTimeoutTimer.setSingleShot(true);
    m_loadBinaryTimeoutTimer.setInterval(3000);

    updateAvailablePorts();
    connect(&m_updatePortsTimer, &QTimer::timeout, this, &Device::updateAvailablePorts);
    connect(&m_waitLineTimeoutTimer, &QTimer::timeout, this, [this] ()
    {
        emit errorMessage(QObject::tr("Not response"));
        stopWaitLine();
    });
    connect(&m_loadBinaryTimeoutTimer, &QTimer::timeout, this, [this] ()
    {
        emit errorMessage(QObject::tr("Not response"));
        stopLoadBinary();
    });

    m_updatePortsTimer.start();
}

QStringList const& Device::availablePorts() const
{
    return m_availablePorts;
}

int Device::selectedPort() const
{
    return m_selectedPort;
}

bool Device::opened() const
{
    return m_serialPort.isOpen();
}

bool Device::busy() const
{
    return m_busy;
}

int Device::baudRate() const
{
    return m_baudRate;
}

int Device::bottomBaudRate() const
{
    return 2400;
}

int Device::topBaudRate() const
{
    return 1000000;
}

QStringList const& Device::logsMonths() const
{
    return m_logsMonths;
}

QVariantList const& Device::selectedLogsInfo() const
{
    return m_selectedLogsInfo;
}

void Device::open()
{
    if (opened())
    {
        return;
    }


    m_serialPort.setPort(QSerialPortInfo::availablePorts().at(selectedPort()));
    m_serialPort.setBaudRate(m_baudRate);

    qInfo() << "Device open required:" << m_serialPort.portName()
            << "baudrate:" << m_serialPort.baudRate();

    m_serialPort.open(QSerialPort::ReadWrite);

    openedChanged(opened());

    if (opened())
    {
        toSafeMode();
        qInfo() << "Device opened";
    }
    else
    {
        emit errorMessage(m_serialPort.errorString());
    }
}

void Device::close()
{
    if (!opened())
    {
        return;
    }

    stopWaitLine();

    m_serialPort.close();
    openedChanged(opened());

    reset();

    qInfo() << "Device closed";
}

void Device::restart()
{
    if (!opened())
    {
        return;
    }

    qInfo("Device restart required");

    QString respone = waitLine("restart\n");
    if (respone == "disconnect")
    {
        setBusy(true);
        QEventLoop loop;
        QTimer::singleShot(1000, this, [&loop] () { loop.exit(); });
        loop.exec();
        setBusy(false);

        toSafeMode();
        reset();
        qInfo() << "Device restarted";
    }
    else
    {
        emit errorMessage(QObject::tr("Failed to restart. Response: ") + respone);
        close();
    }
}

void Device::syncDateTime()
{
    if (!opened())
    {
        return;
    }

    qInfo("Device's date and time sync required");

    QDateTime dateTime = QDateTime::currentDateTime();

    QString dateStr;
    QString timeStr;
    dateStr.sprintf("%02u-%02u-%02u",
                    dateTime.date().day(),
                    dateTime.date().month(),
                    dateTime.date().year() - 2000);
    timeStr.sprintf("%02u:%02u:%02u",
                    dateTime.time().hour(),
                    dateTime.time().minute(),
                    dateTime.time().second());

    QString cmd = "set date " + dateStr + "\n" + "set time " + timeStr + "\n";
    m_serialPort.write(cmd.toLatin1());
    m_serialPort.flush();

    qInfo("Device's date and time synced");
}

void Device::getLogsMonths()
{
    if (!opened())
    {
        return;
    }

    qInfo() << "Get months logs info required";
    m_logsMonths = QString(waitLine("get logs_months")).split(' ');
    emit logsMonthsChanged(m_logsMonths);
    qInfo() << "Months logs info loaded";
}

void Device::getLogsInfoByMonth(QString const& month)
{
    if (!opened())
    {
        return;
    }

    loadLogsInfoByMonth(month);

    m_selectedLogsInfo = m_loadedLogsInfo.value(month, {});
    emit selectedLogsInfoChanged(m_selectedLogsInfo);
}

QString Device::getTimestamp()
{
    if (!opened())
    {
        return "";
    }

    qInfo() << "Timestamp required";

    QDateTime dateTime;

    dateTime.setDate(dateFromString(getDate()));
    dateTime.setTime(timeFromString(getTime()));

    qInfo() << "Timestamp loaded";

    return dateTime.toString();
}

QString Device::getUserInfo()
{
    if (!opened())
    {
        return "";
    }

    qInfo() << "User info required";

    auto codec = QTextCodec::codecForName(CODEC_TEXT);

    if (codec == nullptr)
    {
        emit errorMessage(QObject::tr("Unsupported codec: ") + CODEC_TEXT);
        return "";
    }

    QByteArray userInfo = waitLine("get user_info");

    qInfo() << "User info loaded";

    QString text = codec->toUnicode(userInfo);

    text = text.replace("\\\\", "\\");
    text = text.replace("\\\"", "\"");
    text = text.replace("\\t", "\t");
    text = text.replace("\\n", "\n");

    return text;
}

QString Device::getVersionInfo()
{
    if (!opened())
    {
        return "";
    }

    qInfo() << "Version info required";
    auto result = waitLine("get version_info");
    qInfo() << "Version info loaded";

    return result;
}

void Device::loadSelectedLogs()
{
    if (!opened() || m_selectedLogs.isEmpty())
    {
        return;
    }

    emit progressDialog(QObject::tr("Load selected logs(%1)").arg(m_selectedLogs.size()));
    loadLogs(m_selectedLogs.toList());
    emit progressDialog({});
}

void Device::loadLogsByMonth(QString const& month)
{
    if (!opened())
    {
        return;
    }

    loadLogsInfoByMonth(month);

    QList<LogInfoProxy> logs{};
    {
        auto logsInfo = m_loadedLogsInfo.value(month);
        logs.reserve(logs.size());
        for (auto const& logInfo : logsInfo)
        {
            logs.append(logInfo.value<LogInfoProxy>());
        }
    }

    emit progressDialog(QObject::tr("Load logs by month: %1 (%2)").arg(month).arg(logs.size()));
    loadLogs(logs);
    emit progressDialog({});
}

void Device::loadAllLogs()
{
    if (!opened())
    {
        return;
    }

    getLogsMonths();

    QList<LogInfoProxy> logs{};
    {
        for (auto const& month : logsMonths())
        {
            loadLogsInfoByMonth(month);
        }
        for (auto const& month : m_loadedLogsInfo.keys())
        {
            for (auto const& logInfo : m_loadedLogsInfo.value(month))
            {
                logs.append(logInfo.value<LogInfoProxy>());
            }
        }
    }

    emit progressDialog(QObject::tr("Load all logs (%1)").arg(logs.size()));
    loadLogs(logs);
    emit progressDialog({});
}

void Device::abortLoadLogs()
{
    if (m_loadLogs)
    {
        qInfo("Abort load logs required");
        m_loadLogs = false;
    }
}

void Device::setUserInfo(QString const& userInfo)
{
    if (!opened())
    {
        return;
    }

    qInfo() << "Set user info requered:" << userInfo;

    auto codec = QTextCodec::codecForName(CODEC_TEXT);

    if (codec == nullptr)
    {
        emit errorMessage(QObject::tr("Unsupported codec: ") + CODEC_TEXT);
        return;
    }

    QString escaped = {};

    for (auto const& c : userInfo)
    {
        if (c == '\"')
        {
            escaped += "\\\"";
        }
        else if (c == '\n')
        {
            escaped += "\\n";
        }
        else if (c == '\t')
        {
            escaped += "\\t";
        }
        else if (c == '\\')
        {
            escaped += "\\\\";
        }
        else
        {
            escaped += c;
        }
    }

    auto data = codec->fromUnicode(escaped);

    setBusy(true);
    QString result = waitLine(QString("set user_info %1").arg(data.size()));
    if (result != "ready_read")
    {
        emit errorMessage(QObject::tr("Cannot save user info"));
        setBusy(false);
        return;
    }

    m_serialPort.write(data);
    m_serialPort.flush();

    result = waitLine();
    if (result != "success")
    {
        emit errorMessage(QObject::tr("Cannot save user info"));
        setBusy(false);
        return;
    }

    qInfo() << "User info saved";
    setBusy(false);
}

void Device::setFolderLogLoad(QUrl const& url)
{
    m_logLoadDir = url;
}

bool Device::isSelected(QVariant const& info) const
{
    return m_selectedLogs.contains(info.value<LogInfoProxy>());
}

int Device::select(QVariant const& info)
{
    m_selectedLogs.insert(info.value<LogInfoProxy>());
    return m_selectedLogs.size();
}

int Device::unselect(QVariant const& info)
{
    m_selectedLogs.remove(info.value<LogInfoProxy>());
    return m_selectedLogs.size();
}

int Device::unselectAll()
{
    m_selectedLogs.clear();
    return m_selectedLogs.size();
}

void Device::setSelectedPort(int selectedPort)
{
    if (m_selectedPort == selectedPort)
    {
        return;
    }

    m_selectedPort = selectedPort;
    emit selectedPortChanged(m_selectedPort);
}

void Device::setBaudRate(int baudRate)
{
    if (m_baudRate == baudRate)
    {
        return;
    }

    m_baudRate = baudRate;
    emit baudRateChanged(m_baudRate);
}

void Device::updateAvailablePorts()
{
    QString const portName = m_availablePorts.empty() ? "" : m_availablePorts.at(selectedPort());

    m_availablePorts.clear();

    for (auto const& port : QSerialPortInfo::availablePorts())
    {
        m_availablePorts << port.portName();
    }

    emit availablePortsChanged(m_availablePorts);

    int newIndex = 0;
    if (!portName.isEmpty())
    {
        for (int i = 0; i < m_availablePorts.size(); i++)
        {
            if (m_availablePorts.at(i) == portName)
            {
                newIndex = i;
                break;
            }
        }
    }
    setSelectedPort(newIndex);
}

void Device::onAboutClose()
{
    emit openedChanged(opened());
}

void Device::onReadyRead()
{
    if (m_waitLineBuffer)
    {
        m_waitLineBuffer->append(m_serialPort.readAll());
        m_waitLineTimeoutTimer.start();

        if (!m_waitLineBuffer->isEmpty() && m_waitLineBuffer->back() == '\n')
        {
            m_waitLineBuffer->remove(m_waitLineBuffer->size() - 1, 1);
            stopWaitLine();
        }
    }
    else if (m_loadBinaryBuffer)
    {
        auto readed = m_serialPort.readAll();
        m_loadBinaryBuffer->append(readed);
        m_loadBinaryTimeoutTimer.start();

        emit binaryDataLoaded(readed.size());

        if (m_loadBinaryBuffer->size() == static_cast<int>(m_sizeToLoadBinaryData))
        {
            stopLoadBinary();
        }
    }
    else
    {
        m_messageBuffer.append(m_serialPort.readAll());
        if (!m_messageBuffer.isEmpty() && m_messageBuffer.back() == '\n')
        {
            onMessage(m_messageBuffer);
            m_messageBuffer.clear();
        }
    }
}

void Device::onMessage(QString const& message)
{
    qDebug() << message;
}

void Device::reset()
{
    m_logsMonths.clear();
    emit logsMonthsChanged(m_logsMonths);

    m_selectedLogsInfo.clear();
    emit selectedLogsInfoChanged(m_selectedLogsInfo);

    m_loadedLogsInfo.clear();
    m_selectedLogs.clear();
}

QDate Device::dateFromString(QString const& string)
{
    QStringList dateStrs = string.split('-');

    if (dateStrs.size() != 3)
    {
        return {};
    }

    return QDate(dateStrs.at(2).toInt() + 2000,
                 dateStrs.at(1).toInt(),
                 dateStrs.at(0).toInt());
}

QTime Device::timeFromString(QString const& string)
{
    QStringList timeStrs = string.split(':');

    if (timeStrs.size() != 3)
    {
        return {};
    }

    return QTime(timeStrs.at(0).toInt(),
                 timeStrs.at(1).toInt(),
                 timeStrs.at(2).toInt());
}

void Device::setBusy(bool busy)
{
    if (m_busy == busy)
    {
        return;
    }

    m_busy = busy;
    emit busyChanged(busy);
}

QByteArray Device::waitLine(QString const& cmd)
{
    QByteArray buffer = {};
    m_waitLineBuffer = &buffer;

    setBusy(true);

    QEventLoop loop;
    m_waitLineLoop = &loop;

    if (!cmd.isEmpty())
    {
        m_serialPort.write((cmd + "\n").toLatin1());
        m_serialPort.flush();
    }

    if (m_waitLineLoop)
    {
        m_waitLineTimeoutTimer.start();
        loop.exec();
    }

    m_waitLineLoop = nullptr;

    setBusy(false);

    m_waitLineBuffer = nullptr;
    return buffer;
}

QByteArray Device::loadBinary(unsigned long bytes, QString const& cmd)
{
    m_sizeToLoadBinaryData = bytes;

    QByteArray buffer = {};
    m_loadBinaryBuffer = &buffer;

    setBusy(true);

    QEventLoop loop;
    m_loadBinaryLoop = &loop;

    if (!cmd.isEmpty())
    {
        m_serialPort.write((cmd + "\n").toLatin1());
        m_serialPort.flush();
    }

    if (m_loadBinaryLoop)
    {
        m_loadBinaryTimeoutTimer.start();
        loop.exec();
    }

    m_loadBinaryLoop = nullptr;

    setBusy(false);

    m_loadBinaryBuffer = nullptr;
    return buffer;
}

void Device::stopWaitLine()
{
    m_waitLineTimeoutTimer.stop();
    if (m_waitLineLoop)
    {
        m_waitLineLoop->exit();
    }
}

void Device::stopLoadBinary()
{
    m_sizeToLoadBinaryData = 0;
    m_loadBinaryTimeoutTimer.stop();
    if (m_loadBinaryLoop)
    {
        m_loadBinaryLoop->exit();
    }
}

void Device::waitReadyWrite()
{
    QEventLoop loop;
    connect(&m_serialPort, &QSerialPort::bytesWritten, &loop, [&loop]
    {
        loop.exit();
    });
    loop.exec();
}

void Device::toSafeMode()
{
    QString result = waitLine("open_safe");

    if (result != "ready")
    {
        emit errorMessage(QObject::tr("Cannot open safe mode"));
        close();
    }
}

QString Device::getDate()
{
    return waitLine("get date");
}

QString Device::getTime()
{
    return waitLine("get time");
}

void Device::loadLogsInfoByMonth(const QString& month)
{
    qInfo() << "Load logs info by month required:" << month;

    if (!m_loadedLogsInfo.contains(month))
    {
        QVariantList infoProxies{};
        auto infoList = QString(waitLine("get logs_by_month " + month))
                        .split(' ', QString::SkipEmptyParts);
        infoProxies.reserve(infoList.size());

        for (auto const& info : infoList)
        {
            QDateTime dateTime;
            LogInfoProxy proxy{};

            QStringList parts = info.split('_', QString::SkipEmptyParts);

            if (parts.empty())
            {
                continue;
            }

            if (parts.size() > 0)
            {
                proxy.setFile(parts[0]);
            }
            if (parts.size() > 1)
            {
                proxy.setSize(parts[1].toULong());
            }
            if (parts.size() > 2)
            {
                dateTime.setDate(dateFromString(parts[2]));
            }
            if (parts.size() > 3)
            {
                dateTime.setTime(timeFromString(parts[3]));
            }

            proxy.setModified(dateTime);
            proxy.setMonth(month);
            infoProxies << QVariant::fromValue(proxy);
        }

        m_loadedLogsInfo[month] = infoProxies;
    }

    qInfo() << "Logs info by month loaded:" << month;
}

void Device::loadLog(LogInfoProxy const& info)
{
    QDir(m_logLoadDir.path()).mkdir(info.month());

    QFile file(QDir(m_logLoadDir.path()).filePath(info.fullName()));
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    Q_ASSERT_X(file.isOpen(), "Load log", qUtf8Printable(file.errorString()));

    qInfo() << "Load log" << info.fullName() << "size:" << info.size();
    file.write(loadBinary(info.size(), "get log " + info.fullName()));
    qInfo() << "Load:" << info.fullName() << "loaded";

    file.close();
}

void Device::loadLogs(QList<LogInfoProxy> const& logs)
{
    if (!m_logLoadDir.isValid() || !QDir(m_logLoadDir.path()).exists())
    {
        emit errorMessage("Cannot resolve apth dir for load logs");
        return;
    }

    int i = 0;
    unsigned long sumBytes = 0;
    unsigned long sumBytesInProgress = 0;
    unsigned long loadedBytes = 0;
    int maxLenFileName = 0;
    LogInfoProxy info{};

    m_loadLogs = true;


    for (auto const& log : logs)
    {
        sumBytes += log.size();
        if (log.fullName().length() > maxLenFileName)
        {
            maxLenFileName = log.fullName().length();
        }
    }

    auto textProgressEmit = [&, this] (unsigned long readed)
    {
        sumBytesInProgress += readed;
        auto r = sumBytes - sumBytesInProgress;

        auto rmsec = ((r * 18 * 1000) / m_baudRate);
        loadedBytes += readed;

        emit ettChanged(QTime::fromMSecsSinceStartOfDay(rmsec).toString("HH:mm:ss.zzz"));
        emit progressChanged(static_cast<qreal>(sumBytesInProgress) / sumBytes);

        QString toloadLogs = QString::number(logs.size());
        QString toLoadBytes = QString::number(info.size());

        emit textProgressChanged(QObject::tr("Status: [%1/%2] - load log %3: %4/%5")
                                 .arg(i + 1, toloadLogs.length(), 10, QChar('0'))
                                 .arg(toloadLogs)
                                 .arg(info.fullName(), maxLenFileName)
                                 .arg(loadedBytes, toLoadBytes.length(), 10, QChar('0'))
                                 .arg(toLoadBytes));
    };

    auto connection = connect(this, &Device::binaryDataLoaded, this, textProgressEmit);
    for (; i < logs.size() && m_loadLogs; i++)
    {
        info = logs[i];
        loadedBytes = 0;
        textProgressEmit(loadedBytes);
        loadLog(info);
    }
    disconnect(connection);
    m_loadLogs = false;
}
