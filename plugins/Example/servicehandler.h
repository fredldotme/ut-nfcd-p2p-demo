#ifndef SERVICEHANDLER_H
#define SERVICEHANDLER_H

#include <QObject>
#include <QDBusContext>
#include <QDBusAbstractAdaptor>
#include <QDBusUnixFileDescriptor>
#include <QDBusObjectPath>

class ServiceHandler : public QObject, QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.sailfishos.nfc.LocalService")

public:
    explicit ServiceHandler(QObject *parent = nullptr);

// org.sailfish.nfc.LocalService interfaces
public Q_SLOTS:
    bool Accept(quint32 rsap, QDBusUnixFileDescriptor fd);
    void DatagramReceived(quint32 rsap, QByteArray data);
    void PeerArrived(QDBusObjectPath path);
    void PeerLeft(QDBusObjectPath path);

// Used to pass received fds on to other objects
Q_SIGNALS:
    void fdReceived(int);

};

#endif // SERVICEHANDLER_H
