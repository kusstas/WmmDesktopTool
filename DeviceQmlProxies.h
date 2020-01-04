#pragma once

#include <QObject>
#include <QList>

#include "DeviceCommon.h"

class SettingsQmlProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int minSerializedDataPage READ minSerializedDataPage CONSTANT)
    Q_PROPERTY(int maxSerializedDataPage READ maxSerializedDataPage CONSTANT)

    Q_PROPERTY(int minPwmClock READ minPwmClock CONSTANT)
    Q_PROPERTY(int maxPwmClock READ maxPwmClock CONSTANT)

    Q_PROPERTY(int minModulationClock READ minModulationClock CONSTANT)
    Q_PROPERTY(int maxModulationClock READ maxModulationClock CONSTANT)

    Q_PROPERTY(int minPwmControlTimerClock READ minPwmControlTimerClock CONSTANT)
    Q_PROPERTY(int maxPwmControlTimerClock READ maxPwmControlTimerClock CONSTANT)

    Q_PROPERTY(int minGeneralClock READ minGeneralClock CONSTANT)
    Q_PROPERTY(int maxGeneralClock READ maxGeneralClock CONSTANT)

    Q_PROPERTY(bool invalid READ invalid WRITE setInvalid NOTIFY invalidChanged)
    Q_PROPERTY(int serializedDataPage READ serializedDataPage WRITE setSerializedDataPage NOTIFY serializedDataPageChanged)
    Q_PROPERTY(int pwmClock READ pwmClock WRITE setPwmClock NOTIFY pwmClockChanged)
    Q_PROPERTY(int modulationClock READ modulationClock WRITE setModulationClock NOTIFY modulationClockChanged)
    Q_PROPERTY(int pwmControlTimerClock READ pwmControlTimerClock WRITE setPwmControlTimerClock NOTIFY pwmControlTimerClockChanged)
    Q_PROPERTY(int generalClock READ generalClock WRITE setGeneralClock NOTIFY generalClockChanged)

public:
    explicit SettingsQmlProxy(Settings const& settings, QObject* parent = nullptr);

    Settings toDeviceData() const;

    int minSerializedDataPage() const;
    int maxSerializedDataPage() const;
    int minPwmClock() const;
    int maxPwmClock() const;
    int minModulationClock() const;
    int maxModulationClock() const;
    int minPwmControlTimerClock() const;
    int maxPwmControlTimerClock() const;
    int minGeneralClock() const;
    int maxGeneralClock() const;

    bool invalid() const;
    int serializedDataPage() const;

    int pwmClock() const;
    int modulationClock() const;
    int pwmControlTimerClock() const;
    int generalClock() const;

public slots:
    void setInvalid(bool invalid);
    void setSerializedDataPage(int serializedDataPage);
    void setPwmClock(int pwmClock);
    void setModulationClock(int modulationClock);
    void setPwmControlTimerClock(int pwmControlTimerClock);
    void setGeneralClock(int generalClock);

signals:
    void invalidChanged(bool invalid);
    void serializedDataPageChanged(int serializedDataPage);
    void pwmClockChanged(int pwmClock);
    void modulationClockChanged(int modulationClock);
    void pwmControlTimerClockChanged(int pwmControlTimerClock);
    void generalClockChanged(int generalClock);

private:
    bool m_invalid = false;
    int m_serializedDataPage = 0;
    int m_pwmClock = 0;
    int m_modulationClock = 0;
    int m_pwmControlTimerClock = 0;
    int m_generalClock = 0;
};

class PresetQmlProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList modes READ modes CONSTANT)
    Q_PROPERTY(int minValue READ minValue CONSTANT)
    Q_PROPERTY(int maxValue READ maxValue CONSTANT)

    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit PresetQmlProxy(Preset const& data, QObject* parent = nullptr);

    Preset toDeviceData() const;

    QStringList modes() const;
    int minValue() const;
    int maxValue() const;
    int mode() const;
    int value() const;

public slots:
    void setMode(int mode);
    void setValue(int value);

signals:
    void modeChanged(int mode);
    void valueChanged(int value);

private:
    int m_mode = 0;
    int m_value = 0;
};

using PresetQmlProxyPtr = PresetQmlProxy*;

class ChannelQmlProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<PresetQmlProxyPtr> presets READ presets CONSTANT)
    Q_PROPERTY(int currentPreset READ currentPreset WRITE setCurrentPreset NOTIFY currentPresetChanged)

public:
    explicit ChannelQmlProxy(ChannelSerializedData const& data, QObject* parent = nullptr);

    ChannelSerializedData toDeviceData() const;

    QList<PresetQmlProxyPtr> presets() const;
    int currentPreset() const;

public slots:
    void setCurrentPreset(int currentPreset);

signals:
    void currentPresetChanged(int currentPreset);

private:
    QList<PresetQmlProxyPtr> m_presets{};
    int m_currentPreset = 0;
};

using ChannelQmlProxyPtr = ChannelQmlProxy*;

class UserPreferencesQmlProxy : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<ChannelQmlProxyPtr> channels READ channels CONSTANT)
    Q_PROPERTY(int overwrites READ overwrites WRITE setOverwrites NOTIFY overwritesChanged)

public:
    explicit UserPreferencesQmlProxy(SerializedData const& data, QObject* parent = nullptr);

    SerializedData toDeviceData() const;

    QList<ChannelQmlProxyPtr> channels() const;
    int overwrites() const;

public slots:
    void setOverwrites(int overwrites);

signals:
    void overwritesChanged(int overwrites);

private:
    QList<ChannelQmlProxyPtr> m_channels{};
    int m_overwrites = 0;
};

Q_DECLARE_METATYPE(PresetQmlProxyPtr)
Q_DECLARE_METATYPE(ChannelQmlProxyPtr)
