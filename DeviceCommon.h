#pragma once

#include <cstdint>

static constexpr unsigned int PWM_MIN_FREQ                      = 1000;
static constexpr unsigned int PWM_MAX_FREQ                      = 200000;
static constexpr unsigned int MODULATION_MIN_FREQ               = 500;
static constexpr unsigned int MODULATION_MAX_FREQ               = 50000;
static constexpr unsigned int PWM_CONTROLS_TIMER_MIN_FREQ       = 10;
static constexpr unsigned int PWM_CONTROLS_TIMER_MAX_FREQ       = 2000;

static constexpr int GENERAL_CLOCK_MIN_FREQ            = 8000;
static constexpr int GENERAL_CLOCK_MAX_FREQ            = 160000000;

static constexpr int EEPROM_PAGE_SIZE                = 64;
static constexpr int EEPROM_TOTAL_SIZE               = __INT16_MAX__;
static constexpr int EEPROM_MAX_PAGE_OVERWRITES      = 1000000;

static constexpr int EEPROM_SETTINGS_PAGE            = 0x0000;
static constexpr int EEPROM_USER_PREF_START_PAGE     = (EEPROM_SETTINGS_PAGE + EEPROM_PAGE_SIZE);
static constexpr int EEPROM_USER_PREF_END_PAGE       = (EEPROM_TOTAL_SIZE - EEPROM_PAGE_SIZE);

static constexpr int COUNT_CHANNELS = 2;
static constexpr int COUNT_MODES = 5;
static constexpr int COUNT_PRESETS = 4;
static constexpr int MIN_VALUE = 1;
static constexpr int MAX_VALUE = 100;
static constexpr int BAUDRATE = 115200;

static constexpr int YEAR_SHIFT = 2000;

enum class Command : uint8_t
{
    None,
    GetDateTime,
    GetSettings,
    GetUserPreferences,
    SetDateTime,
    SetSettings,
    SetUserPreferences,
    ResetSettings,
    ResetUserPreferences,
    Max
};

enum class Mode : uint8_t
{
    Welding,
    Cutting,
    Coagulation,
    Auto,
    Modulation,
};

struct Time
{
    unsigned int hour;
    unsigned int minutes;
    unsigned int seconds;
} __attribute__((packed));

struct Date
{
    unsigned int day;
    unsigned int week;
    unsigned int month;
    unsigned int year;
} __attribute__((packed));

struct DateTime
{
    Date date;
    Time time;
} __attribute__((packed));

struct Settings
{
    bool invalid;
    uint16_t serializedDataPage;
    uint32_t pwmClock;
    uint32_t modulationClock;
    uint32_t pwmControlTimerClock;
    uint32_t generalClock;
} __attribute__((packed));

struct Preset
{
    Mode mode;
    uint8_t value;
} __attribute__((packed));

struct ChannelSerializedData
{
    Preset presets[COUNT_PRESETS] = {};
    uint8_t currentPreset = 0;
} __attribute__((packed));

struct SerializedData
{
    uint32_t overwrites = 0;
    ChannelSerializedData channels[COUNT_CHANNELS] = {};
} __attribute__((packed));
