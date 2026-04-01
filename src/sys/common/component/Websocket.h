//
// Created by 86150 on 2026/3/24.
//
#pragma once
#include <QMutex>
#include <QObject>
#include <QUrl>
#include <QWebsocket>

class Websocket : public QObject
{
    Q_OBJECT

public:
    static Websocket& instance()
    {
        static Websocket ws;
        return ws;
    }

    void open()
    {
        QMutexLocker locker(&m_mutex);
        m_webSocket.open(QUrl(QStringLiteral("ws://localhost:8080/ws/notification")));
    }

    void abort()
    {
        QMutexLocker locker(&m_mutex);
        m_webSocket.abort();
    }


    void sendTextMessage(const QString& message)
    {
        QMutexLocker locker(&m_mutex);
        if (m_webSocket.state() == QAbstractSocket::ConnectedState)
        {
            m_webSocket.sendTextMessage(message);
        }
    }

signals:
    void connected();
    void disconnected();
    void textMessageReceived(const QString& message);

private:
    Websocket()
    {
        connect(&m_webSocket, &QWebSocket::connected, this, &Websocket::connected);
        connect(&m_webSocket, &QWebSocket::disconnected, this, &Websocket::disconnected);
        connect(&m_webSocket, &QWebSocket::textMessageReceived,
                this, &Websocket::textMessageReceived);
    }

    ~Websocket() = default;

    Websocket(const Websocket&) = delete;
    Websocket& operator=(const Websocket&) = delete;

private:
    QMutex m_mutex;
    QWebSocket m_webSocket;
};
