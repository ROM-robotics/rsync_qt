#pragma once
#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class RosBridgeClient : public QObject {
    Q_OBJECT
public:
    explicit RosBridgeClient(const QString &robot_ns="", 
        const QString &host = "127.0.0.1", 
        quint16 port = 9090, 
        QObject *parent = nullptr);

    // --------------------------------- MAIN API
    void connectToServer();
    void disconnectFromServer();
    bool isConnected() const { return m_socket.state() == QAbstractSocket::ConnectedState; }

    // --------------------------------- TOPIC SUBSCRIPTIONS
    void subscribeTopic(const QString &topic_name, const QString &msg_type, const QString &ns);
    void unsubscribeTopic(const QString &topic_name, const QString &ns);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &msg);


private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError err);
    
    void onTextMessageReceived(const QString &msg);

    
protected:
        

private:
    void sendJson(const QJsonObject &obj);
    void ensureReconnect();

    // main api variables
    QWebSocket m_socket;
    QString m_robotNamespace;
    QString m_host;
    quint16 m_port{9090};
    QTimer m_reconnectTimer;

    
};
