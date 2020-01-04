#include "DevicesMonitor.h"

#include <QSerialPortInfo>

DevicesMonitor::DevicesMonitor(QObject* parent)
    : QObject(parent)
{
    m_names = getList();
    if (!names().isEmpty())
    {
        m_selectedIndex = 0;
    }

    connect(&m_timer, &QTimer::timeout, this, &DevicesMonitor::onTimeout);
    m_timer.setInterval(1000);
    m_timer.start();
}

QStringList DevicesMonitor::names() const
{
    return m_names;
}

int DevicesMonitor::selectedIndex() const
{
    return m_selectedIndex;
}

void DevicesMonitor::setSelectedIndex(int selectedIndex)
{
    if (m_selectedIndex == selectedIndex)
        return;

    if (selectedIndex >= names().length())
        return;

    m_selectedIndex = selectedIndex;
    emit selectedIndexChanged(m_selectedIndex);
}

QStringList DevicesMonitor::getList()
{
    QStringList names;

    for (auto const& info : QSerialPortInfo::availablePorts())
    {
        names.append(info.portName());
    }

    return names;
}


void DevicesMonitor::onTimeout()
{
    auto newList = getList();

    if (newList != names())
    {
        int newIndex = selectedIndex();

        if (selectedIndex() >= 0)
        {
            newIndex = newList.indexOf(names()[selectedIndex()]);
        }


        bool const indexChanged = newIndex != selectedIndex();

        m_names = newList;
        m_selectedIndex = newIndex;
        if (indexChanged)
        {
            emit selectedIndexChanged(selectedIndex());
        }
        emit namesChanged(names());
    }
}
