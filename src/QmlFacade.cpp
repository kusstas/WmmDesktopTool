#include "QmlFacade.h"
#include "Device.h"

#include <QDebug>
#include <QDesktopServices>

QmlFacade::QmlFacade(QObject* parent)
    : QObject(parent)
    , m_device(new Device(this))
{
    connect(m_device, &Device::errorMessage, this, [this] (QString const& message)
    {
        qCritical() << "Device error:" << message;
        emit showMessage(QObject::tr("Device error!"), message);
    });
}

QmlFacade::~QmlFacade()
{
    device()->close();
}

Device* QmlFacade::device() const
{
    return m_device;
}

void QmlFacade::openUrl(QUrl const& url)
{
    QDesktopServices::openUrl(url);
}
