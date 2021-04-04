#ifndef READER_H
#define READER_H

#include <QObject>
#include <QSocketNotifier>

#include "daemon_interface.h"
#include "adapter_interface.h"

class Reader: public QObject {
    Q_OBJECT

public:
    Reader();
    ~Reader();

    Q_INVOKABLE void listenService();

Q_SIGNALS:
    void bytesReceived(QString bytes);

private:
    OrgSailfishosNfcDaemonInterface* m_daemonIface = nullptr;
    OrgSailfishosNfcAdapterInterface* m_adapterIface = nullptr;

    int m_fd = -1;
    QSocketNotifier* m_readNotifer = nullptr;
    uint m_modeId = 0;
};

#endif
