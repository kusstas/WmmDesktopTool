#ifndef LOGINFOPROXY_H
#define LOGINFOPROXY_H

#include <QObject>
#include <QDateTime>

class LogInfoProxy
{
    Q_GADGET

    Q_PROPERTY(QString file READ file)
    Q_PROPERTY(QString month READ month)
    Q_PROPERTY(QDateTime modified READ modified)
    Q_PROPERTY(QString sizeText READ sizeText)
public:
    LogInfoProxy() = default;

    bool operator == (LogInfoProxy const& info) const;
    bool operator != (LogInfoProxy const& info) const;

    QString const& fullName() const;
    QString const& file() const;
    QString const& month() const;
    QDateTime const& modified() const;
    QString const& sizeText() const;
    unsigned long size() const;

    void setFile(QString const& file);
    void setMonth(QString const& month);
    void setModified(QDateTime const& modified);
    void setSize(unsigned long size);

private:
    QString m_fullName{};
    QString m_file{};
    QString m_month{};
    QDateTime m_modified{};
    QString m_sizeText{};
    unsigned long m_size = 0;
};

uint qHash(LogInfoProxy const& info, uint seed);

#endif // LOGINFOPROXY_H
