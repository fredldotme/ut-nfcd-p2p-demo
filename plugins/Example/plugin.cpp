#include <QtQml>
#include <QtQml/QQmlContext>

#include "plugin.h"
#include "reader.h"
#include "service.h"

void ExamplePlugin::registerTypes(const char *uri) {
    //@uri Example
    qmlRegisterSingletonType<Reader>(uri, 1, 0, "Reader", [](QQmlEngine*, QJSEngine*) -> QObject* { return new Reader; });
    qmlRegisterSingletonType<Service>(uri, 1, 0, "Service", [](QQmlEngine*, QJSEngine*) -> QObject* { return new Service; });
}
