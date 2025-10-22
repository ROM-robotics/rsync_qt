#include "RsyncRunner.h"

#include <QFileInfo>
#include <QProcessEnvironment>

RsyncRunner::RsyncRunner(QObject* parent)
    : QObject(parent) {}

void RsyncRunner::setRemoteDestPath(const QString& path) {
    if (m_remoteDestPath == path)
        return;
    m_remoteDestPath = path;
    emit remoteDestPathChanged();
}

void RsyncRunner::setDefaultUser(const QString& user) {
    if (m_defaultUser == user)
        return;
    m_defaultUser = user;
    emit defaultUserChanged();
}

void RsyncRunner::setSourceRoot(const QString& path) {
    if (m_sourceRoot == path)
        return;
    m_sourceRoot = path;
    emit sourceRootChanged();
}

void RsyncRunner::clearLogs() {
    if (m_logs.isEmpty()) return;
    m_logs.clear();
    emit logsChanged();
}

void RsyncRunner::appendLog(const QString& line) {
    m_logs += line;
    if (!m_logs.endsWith('\n')) m_logs += '\n';
    emit logsChanged();
}

void RsyncRunner::run(const QString& host,
                      const QString& password,
                      const QStringList& items,
                      const QStringList& excludes) {
    if (host.trimmed().isEmpty()) {
        appendLog("[error] Host is empty");
        emit finished(-1);
        return;
    }
    if (items.isEmpty() && excludes.isEmpty()) {
        appendLog("[warning] No selection and no excludes provided — this will sync the entire source root.");
    }

    // Build destination string: user@host:/remote/path
    const QString dest = QString("%1@%2:%3").arg(m_defaultUser, host.trimmed(), m_remoteDestPath);

    // Build rsync args (these are the args passed to the rsync program)
    QStringList rsyncArgs;
    rsyncArgs << "-a" << "-h" << "-v" << "-P" << "--delete";

    // Use the source root as the source; allow excludes for unchecked items
    // Make sure sourceRoot has no trailing slash
    QString source = m_sourceRoot;
    if (source.endsWith('/')) source.chop(1);

    // Add exclude patterns
    for (const QString& ex : excludes) {
        if (!ex.trimmed().isEmpty()) rsyncArgs << QString("--exclude=%1").arg(ex);
    }

    // .git, .gitignore, .gitmodules, .vscode
    rsyncArgs << "--exclude=.git" << "--exclude=.gitignore" << "--exclude=.gitmodules" << "--exclude=.vscode";

    // Add source (sync the whole source root)
    rsyncArgs << source;

    // Destination at end
    rsyncArgs << dest;

    QString program;
    QStringList programArgs;

    // Build the full argv depending on whether a password was provided
    if (!password.trimmed().isEmpty()) {
        // Ensure sshpass is available when a password is supplied
        const int whichRc = QProcess::execute("which", QStringList() << "sshpass");
        if (whichRc != 0) {
            appendLog("[error] 'sshpass' not found on PATH. Install sshpass or use SSH key authentication.");
            appendLog("         Ubuntu/Debian: sudo apt-get update && sudo apt-get install sshpass");
            appendLog("         Fedora: sudo dnf install sshpass (or enable EPEL)");
            appendLog("         macOS (brew): brew install hudochenkov/sshpass/sshpass");
            emit finished(-1);
            return;
        }

        // Use sshpass to provide password to ssh used by rsync
        program = "sshpass";
        // sshpass -p <password> rsync -e "ssh -oStrictHostKeyChecking=no" <rsyncArgs...>
        programArgs << "-p" << password << "rsync" << "-e" << "ssh -oStrictHostKeyChecking=no";
        programArgs += rsyncArgs;
    } else {
        // No password: run rsync directly; use -e to pass ssh with option
        program = "rsync";
        programArgs << "-e" << "ssh -oStrictHostKeyChecking=no";
        programArgs += rsyncArgs;
    }

    QProcess *proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc]() {
        appendLog(QString::fromLocal8Bit(proc->readAllStandardOutput()));
    });
    connect(proc, &QProcess::readyReadStandardError, this, [this, proc]() {
        appendLog(QString::fromLocal8Bit(proc->readAllStandardError()));
    });
    connect(proc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, proc](int code, QProcess::ExitStatus) {
        appendLog(QString("[done] rsync exited with code %1").arg(code));
        if (code == 0) {
            // Show the more descriptive success message requested by the user
            m_status = "rsync copy ok!";
            m_statusColor = "green";
        } else {
            // Map rsync's code 255 to a clearer message per user request
            if (code == 255) {
                m_status = "rsync not work error";
            } else {
                m_status = QString("Error %1").arg(code);
            }
            m_statusColor = "red";
        }
        emit statusChanged();
        proc->deleteLater();
        emit finished(code);
    });

    appendLog(QString("[running] %1 %2").arg(program, programArgs.join(' ')));
    proc->start(program, programArgs);

    if (!proc->waitForStarted(3000)) {
        appendLog(QString("[error] failed to start '%1' (is the program installed and on PATH?)").arg(program));
        appendLog(QString::fromLocal8Bit(proc->readAllStandardError()));
        m_status = "Error: failed to start";
        m_statusColor = "red";
        emit statusChanged();
        proc->deleteLater();
        emit finished(-1);
        return;
    }
}

void RsyncRunner::runRemoteCommand(const QString& host,
                                   const QString& password,
                                   const QString& remotePath,
                                   const QString& command) {
    if (host.trimmed().isEmpty()) {
        appendLog("[error] Host is empty");
        emit finished(-1);
        return;
    }

    // Build ssh command: run the provided command directly on the remote host.
    // NOTE: we intentionally do not prefix with `cd` here — commands should be full/absolute
    // or otherwise handle their own working-directory logic.
    const QString userHost = QString("%1@%2").arg(m_defaultUser, host.trimmed());
    const QString remoteCmd = command;

    QString program;
    QStringList args;

    if (!password.trimmed().isEmpty()) {
        // Ensure sshpass exists
        const int whichRc = QProcess::execute("which", QStringList() << "sshpass");
        if (whichRc != 0) {
            appendLog("[error] 'sshpass' not found on PATH. Install sshpass or use SSH key authentication.");
            emit finished(-1);
            return;
        }
        program = "sshpass";
        args << "-p" << password << "ssh" << "-o" << "StrictHostKeyChecking=no" << userHost << remoteCmd;
    } else {
        program = "ssh";
        args << "-o" << "StrictHostKeyChecking=no" << userHost << remoteCmd;
    }

    QProcess *proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc]() {
        appendLog(QString::fromLocal8Bit(proc->readAllStandardOutput()));
    });
    connect(proc, &QProcess::readyReadStandardError, this, [this, proc]() {
        appendLog(QString::fromLocal8Bit(proc->readAllStandardError()));
    });
    connect(proc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [this, proc](int code, QProcess::ExitStatus) {
        appendLog(QString("[done] remote command exited with code %1").arg(code));
        if (code == 0) {
            m_status = "remote command ok";
            m_statusColor = "green";
        } else {
            m_status = QString("Remote Error %1").arg(code);
            m_statusColor = "red";
        }
        emit statusChanged();
        proc->deleteLater();
        emit finished(code);
    });

    appendLog(QString("[running] %1 %2").arg(program, args.join(' ')));
    proc->start(program, args);
    if (!proc->waitForStarted(3000)) {
        appendLog(QString("[error] failed to start '%1' (is the program installed and on PATH?)").arg(program));
        appendLog(QString::fromLocal8Bit(proc->readAllStandardError()));
        m_status = "Error: failed to start";
        m_statusColor = "red";
        emit statusChanged();
        proc->deleteLater();
        emit finished(-1);
        return;
    }
}
