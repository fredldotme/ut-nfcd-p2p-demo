#include "servicehandler.h"

#include <QDebug>
#include <unistd.h>

ServiceHandler::ServiceHandler(QObject *parent) : QObject(parent)
{

}

bool ServiceHandler::Accept(quint32 rsap, QDBusUnixFileDescriptor fd)
{
    qInfo() << "Accept called, rsap" << rsap << ", fd:" << fd.fileDescriptor();
    int fddup = dup(fd.fileDescriptor());
    Q_EMIT fdReceived(fddup);
    return true;
}

void ServiceHandler::DatagramReceived(quint32 rsap, QByteArray data)
{
    qInfo() << "Datagram received, rsap" << rsap << ", data:" << data;
}

void ServiceHandler::PeerArrived(QDBusObjectPath path)
{
    qInfo() << "Peer arrived at path" << path.path();
}

void ServiceHandler::PeerLeft(QDBusObjectPath path)
{
    qInfo() << "Peer left at path" << path.path();
}
