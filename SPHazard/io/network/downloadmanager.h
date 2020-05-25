#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "QString"
#include "QList"
#include <QtNetwork>
#include <QtCore>
#include <functional>
#include <cstdlib>
#include <iostream>
#include <QSslSocket>


//define listener functions for download progress and finish
class DownloadTask : public QObject
{
    Q_OBJECT


    bool m_CallBackStartSet = false;
    bool m_CallBackProgressSet = false;
    bool m_CallBackFinishedSet = false;
    std::function<void(DownloadTask*)> m_CallBackStart;
    std::function<void(DownloadTask*)> m_CallBackProgress;
    std::function<void(DownloadTask*)> m_CallBackFinish;

    bool m_CallBackAddedSet = false;
    std::function<void(DownloadTask*)> m_CallBackAdd;

public:



    QNetworkReply * Download = nullptr;

    QString Url = "";
    QString file = "";
    QString Password = "";
    QString UserName = "";
    int timeout;
    QByteArray m_Data;
    bool canbesaved = false;


    qint64 bytes_received = 0;
    qint64 bytes_total = 0;

    inline DownloadTask(QString url)
    {
        Url = url;
    }

    inline DownloadTask(QString url, QString file_save, QString username, QString password, int in_timeout)
    {
        Url = url;
        file = file_save;
        UserName = username;
        Password = password;
        timeout = in_timeout;

        QFileInfo fi(file_save);
        if(!fi.dir().exists())
        {
            fi.dir().mkdir(fi.dir().path());
        }
        if(fi.dir().exists())
        {
            canbesaved = true;
        }
    }

    inline bool CanBeSaved()
    {
        return canbesaved;
    }

    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackStart(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackStartSet = true;
        m_CallBackStart = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }
    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackProgress(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackProgressSet = true;
        m_CallBackProgress = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }
    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackFinished(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackFinishedSet = true;
        m_CallBackFinish = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }

    template<typename _Callable1, typename... _Args1>
    inline void SetCallBackAdd(_Callable1&& __f1, _Args1&&... __args1)
    {
        m_CallBackAddedSet = true;
        m_CallBackAdd = std::bind(std::forward<_Callable1>(__f1),std::forward<_Args1>(__args1)...,std::placeholders::_1);
    }


    inline ~DownloadTask()
    {


    }

    inline void OnAdd()
    {
        if(m_CallBackAddedSet)
        {
            m_CallBackAdd(this);
        }
    }


    inline void OnStart()
    {
        m_CallBackStart(this);
    }


public slots:

    inline void downloadProgress(qint64 received,qint64 total)
    {
        bytes_total = total;
        bytes_received = received;
        if(m_CallBackProgressSet)
        {
            m_CallBackProgress(this);
        }

    }

    inline void downloadFinished()
    {
        if(m_CallBackFinishedSet)
        {
            m_Data = Download->readAll();
            m_CallBackFinish(this);
        }
    }

    inline void downloadReadyRead()
    {


    }

    inline void downloadError(QNetworkReply::NetworkError code)
    {
    }
    inline void OnSSLError(const QList<QSslError> &)
    {
    }
    inline void OnRedirected(const QUrl &url)
    {
    }
};

class DownloadManager : QObject
{
    Q_OBJECT

private:

    QNetworkAccessManager *manager;
    QNetworkAccessManager *managernw;
    QList<DownloadTask *> m_TaskList;
    QList<DownloadTask *> m_CurrentList;
    QWaitCondition m_WaitCondition;
    QMutex *m_TaskMutex;
    bool m_DoStop = false;
    QThread *thread;
    QMutex * m_MutexNW;
    QWaitCondition * m_WaitNW;
    QList<DownloadTask *> m_TaskListNW;
    QMutex *m_TaskMutexNW;
    inline void RequestStop()
    {
        m_DoStop = true;
    }

    inline void StartDownload(DownloadTask * t, bool wait = false)
    {


        QNetworkRequest * request = new QNetworkRequest(t->Url);
        t->OnStart();

        QSslConfiguration configSsl = QSslConfiguration::defaultConfiguration();
        configSsl.setProtocol(QSsl::AnyProtocol);
        request->setSslConfiguration(configSsl);
        if(wait)
        {
            t->Download = managernw->get(*request);
        }else {
            t->Download = manager->get(*request);
        }

        connect(t->Download, SIGNAL(downloadProgress(qint64,qint64)),t, SLOT(downloadProgress(qint64,qint64)));
        connect(t->Download, SIGNAL(finished()),t,SLOT(downloadFinished()));
        connect(t->Download, SIGNAL(readyRead()),t,SLOT(downloadReadyRead()));
        connect(t->Download, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),t,&DownloadTask::downloadError);
        connect(t->Download,SIGNAL(sslErrors(const QList<QSslError> &)), t, SLOT(OnSSLError(const QList<QSslError> &)));
        connect(t->Download,SIGNAL(redirected(const QUrl &)), t,SLOT(OnRedirected(const QUrl &)));



    }

public:

    inline DownloadManager()
    {
        m_TaskMutex = new QMutex();
        m_TaskMutexNW = new QMutex();
        thread = QThread::currentThread();

        manager = new QNetworkAccessManager();
        managernw = new QNetworkAccessManager();
        connect(manager, SIGNAL(finished(QNetworkReply*)),this , SLOT(downloadFinished(QNetworkReply*)));
        connect(this,SIGNAL(OnTaskAdded()),this,SLOT(ProcessDownloads()),Qt::ConnectionType::QueuedConnection);
        connect(this,SIGNAL(OnTaskAddedNW()),this,SLOT(ProcessDownloadsNW()),Qt::ConnectionType::QueuedConnection);
    }

    inline ~DownloadManager()
    {

        delete m_TaskMutex;

    }

    inline void Destroy()
    {

        this->StopAllDownloads();

    }


    inline void AddDownload(DownloadTask *t)
    {
        m_TaskMutex->lock();

        t->moveToThread(thread);
        m_TaskList.append(t);

        emit OnTaskAdded();

        m_TaskMutex->unlock();

    }

    inline void AddDownloadAndWait(DownloadTask *t)
    {
        m_MutexNW = new QMutex();
        m_WaitNW = new QWaitCondition();

        m_MutexNW->lock();

        m_TaskMutexNW->lock();

        t->moveToThread(thread);
        m_TaskListNW.append(t);

        std::cout << "download added " << std::endl;
        emit OnTaskAddedNW();

        m_TaskMutexNW->unlock();


        std::cout << "wait... " << std::endl;
        m_WaitNW->wait(m_MutexNW);
        std::cout << "finished " << std::endl;

    }

    inline void StopDownload(DownloadTask *t)
    {
        if(t != nullptr)
        {
            if(t->Download != nullptr)
            {
                if(t->Download->isRunning())
                {
                    t->Download->abort();
                }
                t->Download->deleteLater();
            }
        }
    }

    inline void StopAllDownloads()
    {

    }

    inline bool CheckConnection()
    {

        return false;
    }

public:
signals:
   inline void OnTaskAdded();
    inline void OnTaskAddedNW();

public slots:

    inline void ProcessDownloads()
    {

        m_TaskMutex->lock();


        for(int i = 0; i < m_TaskList.length(); i++)
        {
            m_TaskList.at(i)->OnAdd();
            StartDownload(m_TaskList.at(i));
        }
        m_TaskList.clear();
        m_TaskMutex->unlock();
    }
    inline void ProcessDownloadsNW()
    {
        m_MutexNW->lock();
        m_TaskMutexNW->lock();


        for(int i = 0; i < m_TaskListNW.length(); i++)
        {
            std::cout << "start download"<<std::endl;
            StartDownload(m_TaskListNW.at(i),true);

            //wait untill done
            while(!(m_TaskListNW.at(i)->Download->isFinished()) && (m_TaskListNW.at(i)->Download->error() == QNetworkReply::NetworkError::NoError))
            {
                QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents,25);
                std::cout << "process events"<<std::endl;
            }

            std::cout << "done"<<std::endl;

        }
        m_TaskListNW.clear();
        m_TaskMutexNW->unlock();

        m_MutexNW->unlock();
        m_WaitNW->wakeAll();


    }

    inline void downloadFinished(QNetworkReply * /*p*/)
    {

    }

};


extern DownloadManager * CDownloadManager;

inline static void InitDownloadManager()
{
    CDownloadManager = new DownloadManager();
}

inline static DownloadManager * GetDownloadManager()
{
    return CDownloadManager;

}

inline static void DestroyDownloadManager()
{
    if(CDownloadManager != nullptr)
    {
        delete CDownloadManager;
        CDownloadManager = nullptr;

    };
}

#endif // DOWNLOADMANAGER_H
