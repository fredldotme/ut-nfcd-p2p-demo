#include <QDebug>

#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusUnixFileDescriptor>

#include <unistd.h>

#include "reader.h"
#include "peer_interface.h"

const QString SERVICE_NAME = QStringLiteral("nfcp2pdemo");

// NFC mode values taken straight from nfcd
typedef enum nfc_mode {
    NFC_MODE_NONE           = 0x00,
    /* Polling */
    NFC_MODE_P2P_INITIATOR  = 0x01,
    NFC_MODE_READER_WRITER  = 0x02,
    /* Listening */
    NFC_MODE_P2P_TARGET     = 0x04,
    NFC_MODE_CARD_EMILATION = 0x08
} NFC_MODE;

#define NFC_MODES_P2P (NFC_MODE_P2P_INITIATOR | NFC_MODE_P2P_TARGET)

Reader::Reader() {
    this->m_daemonIface =
            new OrgSailfishosNfcDaemonInterface("org.sailfishos.nfc.daemon",
                                                "/",
                                                QDBusConnection::systemBus(),
                                                this);
}

Reader::~Reader()
{
    if (this->m_readNotifer) {
        delete this->m_readNotifer;
        this->m_readNotifer = nullptr;
    }
}

void Reader::listenService()
{
    // Switch mode to P2P target
    this->m_daemonIface->RequestMode(NFC_MODES_P2P, NFC_MODE_READER_WRITER);

    // Pick the first available adapter
    QString adapterPath;
    QDBusPendingReply<QList<QDBusObjectPath> > reply =
            this->m_daemonIface->GetAdapters();
    reply.waitForFinished();

    if (reply.value().length() <= 0) {
        qWarning() << "No adapter found";
        return;
    }

    adapterPath = reply.value().at(0).path();
    qInfo() << "Using adapter" << adapterPath;

    this->m_adapterIface =
            new OrgSailfishosNfcAdapterInterface("org.sailfishos.nfc.daemon",
                                                 adapterPath,
                                                 QDBusConnection::systemBus(),
                                                 this);

    // Close all open fds when new peers arrive for simplicity,
    // and only care about one connection for now.
    QObject::connect(this->m_adapterIface, &OrgSailfishosNfcAdapterInterface::PeersChanged,
                     [=](const QList<QDBusObjectPath> &peers) {
        qInfo() << "Peers changed";

        // Stop the read notifier
        if (this->m_readNotifer) {
            delete this->m_readNotifer;
            this->m_readNotifer = nullptr;
        }

        // And close the fd
        if (this->m_fd >= 0)
            close(this->m_fd);

        this->m_fd = -1;

        // Pick the first peer available
        for (const QDBusObjectPath& peer : peers) {
            qInfo() << "Checking peer" << peer.path();
            OrgSailfishosNfcPeerInterface peerIface("org.sailfishos.nfc.daemon",
                                                    peer.path(),
                                                    QDBusConnection::systemBus());
            QDBusPendingReply<QDBusUnixFileDescriptor> reply = peerIface.ConnectServiceName(SERVICE_NAME);
            reply.waitForFinished();
            this->m_fd = dup(reply.value().fileDescriptor());
            break;
        }

        // If there is no new fd we're done already
        if (this->m_fd < 0)
            return;

        // Set up socket notifier for read
        this->m_readNotifer = new QSocketNotifier(this->m_fd, QSocketNotifier::Read);
        QObject::connect(this->m_readNotifer, &QSocketNotifier::activated, [=](){
            char buf[4096];
            ssize_t bytesRead = read(this->m_fd, buf, 4096);
            QByteArray bytes(buf, bytesRead);
            Q_EMIT bytesReceived(QString::fromUtf8(bytes));
        });
    });
}
