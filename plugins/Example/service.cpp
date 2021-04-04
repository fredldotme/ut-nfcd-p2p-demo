#include <QDebug>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusPendingReply>

#include <unistd.h>

#include "service.h"

const QString DBUS_HANDLER_PATH = QStringLiteral("/nfcp2pdemo/nfcdep");
const QString SERVICE_NAME = QStringLiteral("nfcp2pdemo");

Service::Service() : QObject(nullptr) {
    this->m_daemonIface =
            new OrgSailfishosNfcDaemonInterface("org.sailfishos.nfc.daemon",
                                                "/",
                                                QDBusConnection::systemBus(),
                                                this);
}

Service::~Service()
{
    if (this->m_handler) {
        QDBusConnection::systemBus().unregisterObject(DBUS_HANDLER_PATH);
        delete this->m_handler;
        this->m_handler = nullptr;
    }
}

void Service::startService(QString text)
{
    if (this->m_handler) {
        QDBusConnection::systemBus().unregisterObject(DBUS_HANDLER_PATH);
        delete this->m_handler;
    }

    this->m_handler = new ServiceHandler();
    const bool registerSuccess =
            QDBusConnection::systemBus().registerObject(DBUS_HANDLER_PATH,
                                                        this->m_handler,
                                                        QDBusConnection::ExportAllSlots);

    QObject::connect(this->m_handler, &ServiceHandler::fdReceived, [=](int fd) {
        QByteArray bytes = text.toUtf8();
        ssize_t writtenBytes = write(fd, bytes, bytes.length());
        qInfo() << "Wrote" << writtenBytes << "bytes";
        close(fd);
    });

    if (!registerSuccess) {
        qWarning() << "Failed to register service.";
        return;
    }

    QDBusPendingReply<uint> reply =
            this->m_daemonIface->RegisterLocalService(QDBusObjectPath(DBUS_HANDLER_PATH),
                                                      SERVICE_NAME);
    reply.waitForFinished();
    if (!reply.isValid()) {
        qWarning() << "Failed to register local service:" << reply.error();
        return;
    }

    this->m_sap = reply.value();
    qInfo() << "Registered service with SAP" << this->m_sap;
}
