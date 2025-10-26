#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include "RsyncRunner.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Parse command-line options for IP and password
    QCommandLineParser parser;
    parser.setApplicationDescription("rsync_qt - lightweight rsync/ssh helper");
    parser.addHelpOption();

    QCommandLineOption ipOption(QStringList() << "i" << "ip",
                                "Server IP address",
                                "ip");
    QCommandLineOption passwordOption(QStringList() << "p" << "password",
                                      "Server password (note: passing passwords on the command line can be insecure)",
                                      "password");

    parser.addOption(ipOption);
    parser.addOption(passwordOption);
    parser.process(app);

    const QString serverIp = parser.value(ipOption);
    const QString serverPassword = parser.value(passwordOption);

    QQmlApplicationEngine engine;
    RsyncRunner rsyncRunner;
    engine.rootContext()->setContextProperty("rsyncRunner", &rsyncRunner);
    // Expose initial values to QML so the UI can pre-fill fields if arguments were provided
    engine.rootContext()->setContextProperty("serverIp", serverIp);
    engine.rootContext()->setContextProperty("serverPassword", serverPassword);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    // Debug output to help diagnose context property availability
    qDebug("serverIp='%s' serverPassword='%s'", qPrintable(serverIp), qPrintable(serverPassword));

    // Load the main QML from the resource path explicitly. Using loadFromModule can sometimes
    // change import resolution/context interactions; loading via the qrc URL keeps the root
    // context predictable so context properties (rsyncRunner, serverIp, serverPassword) are
    // available to the QML root object.
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/rsync_qt/Main.qml")));

    return app.exec();
}
