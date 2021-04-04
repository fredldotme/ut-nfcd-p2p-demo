#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>

#include "daemon_interface.h"
#include "servicehandler.h"

class Service: public QObject {
    Q_OBJECT

public:
    Service();
    ~Service();

    Q_INVOKABLE void startService(QString text);

private:
    OrgSailfishosNfcDaemonInterface* m_daemonIface = nullptr;

    uint m_sap = 0;
    ServiceHandler* m_handler = nullptr;
};

#endif
