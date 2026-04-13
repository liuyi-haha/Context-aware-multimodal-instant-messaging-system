//
// Created by 86150 on 2026/4/12.
//

#pragma once

#include <QSoundEffect>
#include <QUrl>
#include <QThread>
#include <QDebug>
#include <QCoreApplication>

namespace ui::common_widgets
{
    class SoundWorker : public QObject
    {
        Q_OBJECT

    public:
        explicit SoundWorker(QObject* parent = nullptr) : QObject(parent)
        {
        }

    public slots:
        void play()
        {
            if (!m_effect)
            {
                m_effect = new QSoundEffect(this);
                m_effect->setSource(QUrl("qrc:/sound/notification.wav"));
                m_effect->setVolume(1.0f);
                qDebug() << "Sound effect created in thread:" << QThread::currentThread();
            }

            if (m_effect->isLoaded())
            {
                m_effect->play();
                qDebug() << "Playing sound";
            }
        }

    private:
        QSoundEffect* m_effect = nullptr;
    };

    class SoundPlayer : public QObject
    {
        Q_OBJECT

    public:
        static void play()
        {
            static SoundPlayer* instance = nullptr;
            if (!instance)
            {
                instance = new SoundPlayer();
            }
            instance->requestPlay();
        }

    private:
        SoundPlayer(QObject* parent = nullptr) : QObject(parent)
        {
            m_thread = new QThread(this);
            m_worker = new SoundWorker();
            m_worker->moveToThread(m_thread);

            connect(this, &SoundPlayer::playRequested, m_worker, &SoundWorker::play);

            m_thread->start();
        }

        ~SoundPlayer()
        {
            if (m_thread && m_thread->isRunning())
            {
                m_thread->quit();
                m_thread->wait();
            }
        }

        void requestPlay()
        {
            emit playRequested();
        }

    signals:
        void playRequested();

    private:
        QThread* m_thread = nullptr;
        SoundWorker* m_worker = nullptr;
    };
}
