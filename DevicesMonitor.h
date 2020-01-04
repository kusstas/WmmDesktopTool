#pragma once

#include <QObject>
#include <QTimer>

class DevicesMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList names READ names NOTIFY namesChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)

public:
    explicit DevicesMonitor(QObject *parent = nullptr);

    QStringList names() const;
    int selectedIndex() const;

public slots:
    void setSelectedIndex(int selectedIndex);

signals:
    void namesChanged(QStringList names);
    void selectedIndexChanged(int selectedIndex);

private:
    QStringList getList();

    void onTimeout();

private:
    QTimer m_timer{};

    QStringList m_names{};
    int m_selectedIndex = -1;
};

