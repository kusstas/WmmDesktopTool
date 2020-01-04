#include "DeviceQmlProxies.h"

SettingsQmlProxy::SettingsQmlProxy(Settings const& settings, QObject* parent)
    : QObject(parent)
{
    m_invalid = settings.invalid;
    m_serializedDataPage= settings.serializedDataPage;
    m_pwmClock = settings.pwmClock;
    m_modulationClock = settings.modulationClock;
    m_pwmControlTimerClock = settings.pwmControlTimerClock;
    m_generalClock= settings.generalClock;
}

Settings SettingsQmlProxy::toDeviceData() const
{
    Settings settings;

    settings.invalid = m_invalid;
    settings.serializedDataPage= m_serializedDataPage;
    settings.pwmClock = m_pwmClock;
    settings.modulationClock = m_modulationClock;
    settings.pwmControlTimerClock = m_pwmControlTimerClock;
    settings.generalClock= m_generalClock;

    return settings;
}

int SettingsQmlProxy::minSerializedDataPage() const
{
    return EEPROM_USER_PREF_START_PAGE;
}

int SettingsQmlProxy::maxSerializedDataPage() const
{
    return EEPROM_USER_PREF_END_PAGE;
}

int SettingsQmlProxy::minPwmClock() const
{
    return PWM_MIN_FREQ;
}

int SettingsQmlProxy::maxPwmClock() const
{
    return PWM_MAX_FREQ;
}

int SettingsQmlProxy::minModulationClock() const
{
    return MODULATION_MIN_FREQ;
}

int SettingsQmlProxy::maxModulationClock() const
{
    return MODULATION_MAX_FREQ;
}

int SettingsQmlProxy::minPwmControlTimerClock() const
{
    return PWM_CONTROLS_TIMER_MIN_FREQ;
}

int SettingsQmlProxy::maxPwmControlTimerClock() const
{
    return PWM_CONTROLS_TIMER_MAX_FREQ;
}

int SettingsQmlProxy::minGeneralClock() const
{
    return GENERAL_CLOCK_MIN_FREQ;
}

int SettingsQmlProxy::maxGeneralClock() const
{
    return GENERAL_CLOCK_MAX_FREQ;
}

bool SettingsQmlProxy::invalid() const
{
    return m_invalid;
}

int SettingsQmlProxy::serializedDataPage() const
{
    return m_serializedDataPage;
}

int SettingsQmlProxy::pwmClock() const
{
    return m_pwmClock;
}

int SettingsQmlProxy::modulationClock() const
{
    return m_modulationClock;
}

int SettingsQmlProxy::pwmControlTimerClock() const
{
    return m_pwmControlTimerClock;
}

int SettingsQmlProxy::generalClock() const
{
    return m_generalClock;
}

void SettingsQmlProxy::setInvalid(bool invalid)
{
    if (m_invalid == invalid)
        return;

    m_invalid = invalid;
    emit invalidChanged(m_invalid);
}

void SettingsQmlProxy::setSerializedDataPage(int serializedDataPage)
{
    if (m_serializedDataPage == serializedDataPage)
        return;

    if (serializedDataPage < minSerializedDataPage()
        || serializedDataPage > maxSerializedDataPage()
        || serializedDataPage % EEPROM_PAGE_SIZE != 0)
        return;

    m_serializedDataPage = serializedDataPage;
    emit serializedDataPageChanged(m_serializedDataPage);
}

void SettingsQmlProxy::setPwmClock(int pwmClock)
{
    if (m_pwmClock == pwmClock)
        return;

    if (pwmClock < minPwmClock() || pwmClock > maxPwmClock())
        return;

    m_pwmClock = pwmClock;
    emit pwmClockChanged(m_pwmClock);
}

void SettingsQmlProxy::setModulationClock(int modulationClock)
{
    if (m_modulationClock == modulationClock)
        return;

    if (modulationClock < minModulationClock() || modulationClock > maxModulationClock())
        return;

    m_modulationClock = modulationClock;
    emit modulationClockChanged(m_modulationClock);
}

void SettingsQmlProxy::setPwmControlTimerClock(int pwmControlTimerClock)
{
    if (m_pwmControlTimerClock == pwmControlTimerClock)
        return;

    if (pwmControlTimerClock < minPwmControlTimerClock() || pwmControlTimerClock > maxPwmControlTimerClock())
        return;

    m_pwmControlTimerClock = pwmControlTimerClock;
    emit pwmControlTimerClockChanged(m_pwmControlTimerClock);
}

void SettingsQmlProxy::setGeneralClock(int generalClock)
{
    if (m_generalClock == generalClock)
        return;

    if (generalClock < minGeneralClock() || generalClock > maxGeneralClock())
        return;

    m_generalClock = generalClock;
    emit generalClockChanged(m_generalClock);
}

PresetQmlProxy::PresetQmlProxy(Preset const& data, QObject* parent)
    : QObject(parent)
{
    m_mode = static_cast<int>(data.mode);
    m_value = data.value;
}

Preset PresetQmlProxy::toDeviceData() const
{
    Preset preset;

    preset.mode = static_cast<Mode>(m_mode);
    preset.value = static_cast<uint8_t>(m_value);

    return preset;
}

QStringList PresetQmlProxy::modes() const
{
    static QStringList MODES_NAMES =
    {
        "Welding",
        "Cutting",
        "Coagulation",
        "Auto",
        "Modulation",
    };

    return MODES_NAMES;
}

int PresetQmlProxy::minValue() const
{
    return MIN_VALUE;
}

int PresetQmlProxy::maxValue() const
{
    return MAX_VALUE;
}

int PresetQmlProxy::mode() const
{
    return m_mode;
}

int PresetQmlProxy::value() const
{
    return m_value;
}

void PresetQmlProxy::setMode(int mode)
{
    if (m_mode == mode)
        return;

    if (mode < 0 || mode >= modes().size())
        return;

    m_mode = mode;
    emit modeChanged(m_mode);
}

void PresetQmlProxy::setValue(int value)
{
    if (m_value == value)
        return;

    if (value < MIN_VALUE || value > MAX_VALUE)
        return;

    m_value = value;
    emit valueChanged(m_value);
}

ChannelQmlProxy::ChannelQmlProxy(ChannelSerializedData const& data, QObject* parent)
    : QObject(parent)
{
    m_presets.reserve(COUNT_PRESETS);
    for (auto const& preset : data.presets)
    {
        m_presets.append(new PresetQmlProxy(preset, this));
    }

    m_currentPreset = data.currentPreset;
}

ChannelSerializedData ChannelQmlProxy::toDeviceData() const
{
    ChannelSerializedData data;

    for (int i = 0; i < m_presets.size(); i++)
    {
        data.presets[i] = m_presets[i]->toDeviceData();
    }

    data.currentPreset = m_currentPreset;

    return data;
}

QList<PresetQmlProxyPtr> ChannelQmlProxy::presets() const
{
    return m_presets;
}

int ChannelQmlProxy::currentPreset() const
{
    return m_currentPreset;
}

void ChannelQmlProxy::setCurrentPreset(int currentPreset)
{
    if (m_currentPreset == currentPreset)
        return;

    if (currentPreset < 0 || currentPreset >= presets().size())
        return;

    m_currentPreset = currentPreset;
    emit currentPresetChanged(m_currentPreset);
}

UserPreferencesQmlProxy::UserPreferencesQmlProxy(SerializedData const& data, QObject* parent)
    : QObject(parent)
{
    m_channels.reserve(COUNT_CHANNELS);
    for (auto const& channel : data.channels)
    {
        m_channels.append(new ChannelQmlProxy(channel, this));
    }

    m_overwrites = data.overwrites;
}

SerializedData UserPreferencesQmlProxy::toDeviceData() const
{
    SerializedData data;

    for (int i = 0; i < m_channels.size(); i++)
    {
        data.channels[i] = m_channels[i]->toDeviceData();
    }

    data.overwrites = m_overwrites;

    return data;
}

QList<ChannelQmlProxyPtr> UserPreferencesQmlProxy::channels() const
{
    return m_channels;
}

int UserPreferencesQmlProxy::overwrites() const
{
    return m_overwrites;
}

void UserPreferencesQmlProxy::setOverwrites(int overwrites)
{
    if (m_overwrites == overwrites)
        return;

    if (overwrites < 0 || overwrites > EEPROM_MAX_PAGE_OVERWRITES)
        return;

    m_overwrites = overwrites;
    emit overwritesChanged(m_overwrites);
}
