#pragma once

#include <QObject>
#include <QProcess>
#include <QStringList>

class RsyncRunner : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString logs READ logs NOTIFY logsChanged)
    Q_PROPERTY(QString remoteDestPath READ remoteDestPath WRITE setRemoteDestPath NOTIFY remoteDestPathChanged)
    Q_PROPERTY(QString defaultUser READ defaultUser WRITE setDefaultUser NOTIFY defaultUserChanged)
    Q_PROPERTY(QString sourceRoot READ sourceRoot WRITE setSourceRoot NOTIFY sourceRootChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusColor READ statusColor NOTIFY statusChanged)

public:
    explicit RsyncRunner(QObject* parent = nullptr);

    QString logs() const { return m_logs; }

    QString remoteDestPath() const { return m_remoteDestPath; }
    void setRemoteDestPath(const QString& path);

    QString defaultUser() const { return m_defaultUser; }
    void setDefaultUser(const QString& user);

    QString sourceRoot() const { return m_sourceRoot; }
    void setSourceRoot(const QString& path);

    Q_INVOKABLE void clearLogs();

    // Run a remote command over SSH (optionally using sshpass).
    Q_INVOKABLE void runRemoteCommand(const QString& host,
                                      const QString& password,
                                      const QString& remotePath,
                                      const QString& command);

    // Open a local terminal and run ssh (uses sshpass when password provided).
    Q_INVOKABLE void openTerminalSsh(const QString& host, const QString& password);

    // Open a local terminal emulator on the host (no SSH)
    Q_INVOKABLE void openLocalTerminal();

    Q_INVOKABLE void run(const QString& host,
                         const QString& password,
                         const QStringList& items,
                         const QStringList& excludes);

    QString status() const { return m_status; }
    QString statusColor() const { return m_statusColor; }

signals:
    void logsChanged();
    void remoteDestPathChanged();
    void defaultUserChanged();
    void sourceRootChanged();
    void statusChanged();
    void finished(int exitCode);

private:
    void appendLog(const QString& line);

    QString m_logs;
    QString m_remoteDestPath = "/home/mr_robot/Desktop/Git"; // default
    QString m_defaultUser = "mr_robot"; // default
    QString m_sourceRoot = "/home/mr_robot/Desktop/Git/rom_robotics"; // default source
    QString m_status;
    QString m_statusColor;
};
