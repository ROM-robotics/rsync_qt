#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "RsyncRunner.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    RsyncRunner rsyncRunner;
    engine.rootContext()->setContextProperty("rsyncRunner", &rsyncRunner);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("rsync_qt", "Main");

    return app.exec();
}
