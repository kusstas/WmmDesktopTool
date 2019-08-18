#include "LogInfoProxy.h"

bool LogInfoProxy::operator ==(LogInfoProxy const& info) const
{
    return m_file == info.m_file && m_month == info.month();
}

bool LogInfoProxy::operator !=(LogInfoProxy const& info) const
{
    return !(*this == info);
}

QString const& LogInfoProxy::fullName() const
{
    return m_fullName;
}

QString const& LogInfoProxy::file() const
{
    return m_file;
}

QString const& LogInfoProxy::month() const
{
    return m_month;
}

QDateTime const& LogInfoProxy::modified() const
{
    return m_modified;
}

QString const& LogInfoProxy::sizeText() const
{
    return m_sizeText;
}

unsigned long LogInfoProxy::size() const
{
    return m_size;
}

void LogInfoProxy::setFile(QString const& file)
{
    m_file = file;
    m_fullName = m_month + "/" + m_file;
}

void LogInfoProxy::setMonth(QString const& month)
{
    m_month = month;
    m_fullName = m_month + "/" + m_file;
}

void LogInfoProxy::setModified(QDateTime const& modified)
{
    m_modified = modified;
}

void LogInfoProxy::setSize(unsigned long size)
{
    QString const UNITS[] = {"B", "KB", "MB", "GB"};

    m_size = size;

    uint unit = 0;
    unsigned long value = size;
    while (value >= 1024)
    {
        value /= 1024;
        unit++;
    }

    m_sizeText += QString::number(value) + UNITS[unit];
}

uint qHash(LogInfoProxy const& info, uint seed)
{
    return qHash(info.fullName(), seed);
}
