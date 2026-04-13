//
// Created by 86150 on 2026/3/24.
//
#pragma once
#include <QMutex>
#include <QObject>
#include <QUrl>
#include <QWebsocket>
#include "sys/common/component/UserCredentialManager.h"

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
        // 这里要带上token
        auto url = QUrl(
            QString("ws://localhost:8080/ws?token=") + QString(
                sys::common::component::UserCredentialManager::instance().
                getCurrentToken()));
        m_webSocket.open(url);
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
    void binaryMessageReceived(const QByteArray& message);
    void connectFailed(const QString& error);

private:
    Websocket()
    {
        connect(&m_webSocket, &QWebSocket::connected, this, &Websocket::connected);
        connect(&m_webSocket, &QWebSocket::disconnected, this, &Websocket::disconnected);
        connect(&m_webSocket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error)
        {
            Q_UNUSED(error);
            emit connectFailed(m_webSocket.errorString());
        });
        connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
                this, &Websocket::binaryMessageReceived);
    }

    ~Websocket() = default;

    Websocket(const Websocket&) = delete;
    Websocket& operator=(const Websocket&) = delete;

private:
    QMutex m_mutex;
    QWebSocket m_webSocket;
};
