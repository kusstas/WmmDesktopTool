#ifndef QMLFACADE_H
#define QMLFACADE_H

#include <QObject>

class Device;

class QmlFacade : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Device* device READ device CONSTANT)
public:
    explicit QmlFacade(QObject* parent = nullptr);
    ~QmlFacade() override;

    Device* device() const;

signals:
    void showMessage(QString const& title, QString const& message);

private:
    Device* m_device = nullptr;
};

#endif // QMLFACADE_H
