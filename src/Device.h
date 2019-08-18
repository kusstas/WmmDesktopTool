#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QTimer>
#include <QEventLoop>
#include <QSerialPort>
#include <QVariantList>
#include <QHash>
#include <QSet>
#include <QUrl>

#include "LogInfoProxy.h"

class Device : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY availablePortsChanged)
    Q_PROPERTY(int selectedPort READ selectedPort WRITE setSelectedPort
               NOTIFY selectedPortChanged)
    Q_PROPERTY(bool opened READ opened NOTIFY openedChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

    Q_PROPERTY(int baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(int bottomBaudRate READ bottomBaudRate CONSTANT)
    Q_PROPERTY(int topBaudRate READ topBaudRate CONSTANT)

    Q_PROPERTY(QStringList logsMonths READ logsMonths NOTIFY logsMonthsChanged)
    Q_PROPERTY(QVariantList selectedLogsInfo READ selectedLogsInfo NOTIFY selectedLogsInfoChanged)

public:
    explicit Device(QObject *parent = nullptr);

    QStringList const& availablePorts() const;
    int selectedPort() const;
    bool opened() const;
    bool busy() const;

    int baudRate() const;
    int bottomBaudRate() const;
    int topBaudRate() const;

    QStringList const& logsMonths() const;
    QVariantList const& selectedLogsInfo() const;
    bool logsInfoExist() const;

    Q_INVOKABLE void open();
    Q_INVOKABLE void close();
    Q_INVOKABLE void restart();
    Q_INVOKABLE void syncDateTime();
    Q_INVOKABLE void getLogsMonths();
    Q_INVOKABLE void getLogsInfoByMonth(QString const& month);
    Q_INVOKABLE QString getTimestamp();
    Q_INVOKABLE QString getUserInfo();
    Q_INVOKABLE QString getVersionInfo();
    Q_INVOKABLE void loadSelectedLogs();
    Q_INVOKABLE void loadLogsByMonth(QString const& month);
    Q_INVOKABLE void loadAllLogs();
    Q_INVOKABLE void abortLoadLogs();
    Q_INVOKABLE void setUserInfo(QString const& userInfo);
    Q_INVOKABLE void setFolderLogLoad(QUrl const& url);

    Q_INVOKABLE bool isSelected(QVariant const& info) const;
    Q_INVOKABLE int select(QVariant const& info);
    Q_INVOKABLE int unselect(QVariant const& info);
    Q_INVOKABLE int unselectAll();

public slots:
    void setSelectedPort(int selectedPort);
    void setBaudRate(int baudRate);

signals:
    void availablePortsChanged(QStringList const& availablePorts);
    void selectedPortChanged(int selectedPort);
    void openedChanged(bool opened);
    void busyChanged(bool busy);
    void baudRateChanged(int baudRate);
    void logsMonthsChanged(QStringList const& logsMonths);
    void selectedLogsInfoChanged(QVariantList const& selectedLogsInfo);
    void errorMessage(QString const& error);
    void binaryDataLoaded(unsigned long bytes);

    void progressDialog(QString const& title);
    void textProgressChanged(QString const& text);
    void progressChanged(qreal progress);
    void ettChanged(QString const& ett);

private slots:
    void updateAvailablePorts();
    void onAboutClose();
    void onReadyRead();
    void onMessage(QString const& message);

private:
    void reset();
    QDate dateFromString(QString const& string);
    QTime timeFromString(QString const& string);

    void setBusy(bool busy);
    QByteArray waitLine(QString const& cmd = {});
    QByteArray loadBinary(unsigned long bytes, QString const& cmd = {});
    void stopWaitLine();
    void stopLoadBinary();
    void waitReadyWrite();

    void toSafeMode();
    QString getDate();
    QString getTime();
    void loadLogsInfoByMonth(QString const& month);
    void loadLog(LogInfoProxy const& info);
    void loadLogs(QList<LogInfoProxy> const& logs);

private:
    static constexpr char const* CODEC_TEXT = "iso-8859-5";

    QSerialPort m_serialPort{};
    qint32 m_baudRate = 115200;
    int m_selectedPort = 0;

    QByteArray m_messageBuffer{};
    QByteArray* m_waitLineBuffer = nullptr;
    QEventLoop* m_waitLineLoop = nullptr;
    QByteArray* m_loadBinaryBuffer = nullptr;
    QEventLoop* m_loadBinaryLoop = nullptr;
    unsigned long m_sizeToLoadBinaryData = 0;

    QTimer m_updatePortsTimer{};
    QTimer m_waitLineTimeoutTimer{};
    QTimer m_loadBinaryTimeoutTimer{};

    QStringList m_availablePorts{};
    QStringList m_logsMonths{};
    QVariantList m_selectedLogsInfo{};
    QHash<QString, QVariantList> m_loadedLogsInfo{};
    QSet<LogInfoProxy> m_selectedLogs{};
    QUrl m_logLoadDir = {};
    bool m_busy = false;
    bool m_loadLogs = false;
};

#endif // DEVICE_H
