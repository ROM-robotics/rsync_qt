#ifndef ROSE_BRIDGE_CLIENT_HPP
#define ROSE_BRIDGE_CLIENT_HPP

#pragma once
#include <QObject>
#include <QWebSocket>
#include <QTimer>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace rom_dynamics::communication {
    
class RosBridgeClient : public QObject 
{
    Q_OBJECT
public:
    explicit RosBridgeClient(const QString &robot_ns="", 
        const QString &host = "127.0.0.1", 
        quint16 port = 9090, 
        QObject *parent = nullptr);

    // --------------------------------- MAIN API
    bool isConnected() const { return m_socket->state() == QAbstractSocket::ConnectedState; }

signals:
    // --------------------------------- MAIN API
    void connected();
    void disconnected();
    void errorOccurred(const QString &msg);
    
    // --------------------------------- TOPIC SUBSCRIPTIONS
    void receivedTopicMessage(const QString &topic_name, const QJsonObject &msg);

public slots:
    // --------------------------------- MAIN API
    void init();
    void connectToServer();
    void disconnectFromServer();
    
    // --------------------------------- TOPIC SUBSCRIPTIONS
    void subscribeTopic(const QString &topic_name, const QString &msg_type);
    void unsubscribeTopic(const QString &topic_name);
    
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
    // QWebSocket m_socket;
    // ⭐ QWebSocket နှင့် QTimer ကို Pointers များအဖြစ် ပြောင်းပါ။
    QWebSocket *m_socket = nullptr;

    QString m_robotNamespace;
    QString m_host;
    quint16 m_port{9090};

    // QTimer m_reconnectTimer;
    // ⭐ QTimer ကိုလည်း Pointer အဖြစ် ပြောင်းပါ။
    QTimer *m_reconnectTimer = nullptr; 

    
};
}

#endif
