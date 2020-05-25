#ifndef DOWNLOADTOOL_H
#define DOWNLOADTOOL_H

#include "QWidget"
#include "QObject"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QLabel"
#include "QProgressBar"
#include "QToolButton"
#include "QStringListModel"
#include "QItemDelegate"
#include "QListWidgetItem"
#include "QLineEdit"
#include "downloadmanager.h"
#include "iomanager.h"
#include "downloaddialog.h"
#include "QMessageBox"
#include "QProgressBar"
#include "labeledwidget.h"

class DownloadWidget : public QWidget
{

        Q_OBJECT;
public:

    QVBoxLayout * m_MainLayout;
    DownloadTask * m_Task;
    QProgressBar * m_ProgressBar;

    QLabel * m_SizeLabel;
    QLabel * m_StatusLabel;
    bool m_SetSize = false;

    inline DownloadWidget( DownloadTask * task)
    {
        m_Task = task;

        m_MainLayout = new QVBoxLayout();
        m_MainLayout->setSizeConstraint( QLayout::SetFixedSize );
        this->setLayout(m_MainLayout);
        QString nameurl = QUrl(task->Url).fileName();
        QLabel * name = new QLabel(QString(nameurl));
        QFont font = name->font();
        font.setPointSize(13);
        name->setFont(font);

        m_SizeLabel = new QLabel();
        m_StatusLabel = new QLabel();

        m_SizeLabel->setText(" ");
        m_StatusLabel->setText("Waiting to start.. ");

        m_MainLayout->addWidget(name);
        m_MainLayout->addWidget(new QWidgetHDuo(new QLabel("url: " + task->Url),new QLabel("destination: " + task->file)));
        m_MainLayout->addWidget(new QWidgetHDuo(m_StatusLabel,m_SizeLabel));

        m_ProgressBar = new QProgressBar();
        m_MainLayout->addWidget(m_ProgressBar);

        name->setMinimumSize(100,20);


    }

public slots:

    inline void OnDownloadStart(DownloadTask*d)
    {

    }

    inline void OnDownloadProgress(DownloadTask*d)
    {
        if(m_SetSize == false)
        {
            m_StatusLabel->setText("Downloading...");
            m_SetSize = true;
            m_SizeLabel->setText(" Total Size: " +QString::number((int)d->bytes_total/(1024*1024)) + " mb");
        }

        m_ProgressBar->setRange(0,(int)d->bytes_total);
        m_ProgressBar->setValue((int)d->bytes_received);

    }

    inline void OnDownloadFinish(DownloadTask*d)
    {

        //did we succeed?
        if(QNetworkReply::NetworkError::NoError == d->Download->error())
        {

            m_StatusLabel->setText("Error");

        //otherwise mention this to the user
        }else {

            m_StatusLabel->setText("Finished");


        }
    }
};


class DownloadManagerWidget : public QWidget
{

        Q_OBJECT;
public:

    QLineEdit * m_SearchEdit;
    QListWidget * m_DownloadList;
    QVBoxLayout * m_MainLayout;


    QList<QListWidgetItem *> m_ListItems;
    QList<DownloadWidget * > m_ListWidgets;
    QList<QString> m_SearchText;

    DownloadManager * m_DownloadManager;



    inline DownloadManagerWidget()
    {
        m_MainLayout = new QVBoxLayout();
        this->setLayout(m_MainLayout);

        m_DownloadManager = GetDownloadManager();

        QString m_Dir = QCoreApplication::applicationDirPath();

        m_SearchEdit = new QLineEdit();

        connect(m_SearchEdit, SIGNAL(textChanged(QString)),this,SLOT(OnSearchChanged(QString)));


        m_DownloadList = new QListWidget();

        QWidget * tempwidget = new QWidget();
        QHBoxLayout * templayout = new QHBoxLayout();
        tempwidget->setLayout(templayout);

        QIcon iconD;
        iconD.addFile((m_Dir + SPH_FOLDER_ASSETS + "download.png"), QSize(), QIcon::Normal, QIcon::Off);

        QToolButton * m_DownloadButton = new QToolButton();
        m_DownloadButton->setIcon(iconD);
        m_DownloadButton->setText("New Download");
        m_DownloadButton->setIconSize(QSize(22,22));
        m_DownloadButton->resize(22,22);
        m_DownloadButton->setEnabled(true);

        connect(m_DownloadButton,&QToolButton::clicked,this,&DownloadManagerWidget::OnAddDownload);

        templayout->addWidget(m_DownloadButton);
        templayout->addWidget(m_SearchEdit);


        m_MainLayout->addWidget(tempwidget);
        m_MainLayout->addWidget(m_DownloadList);



    }
    inline DownloadWidget *AddDownloadTaskToList(DownloadTask * task)
    {

        QListWidgetItem * item = new QListWidgetItem();
        DownloadWidget * dw = new DownloadWidget(task);

        item->setSizeHint(dw->sizeHint());
        m_DownloadList->addItem(item);
        m_DownloadList->setItemWidget(item,dw);
        m_SearchText.append(task->Url + " " + task->file);

        task->SetCallBackStart(&DownloadWidget::OnDownloadStart,dw);
        task->SetCallBackProgress(&DownloadWidget::OnDownloadProgress,dw);
        task->SetCallBackFinished(&DownloadWidget::OnDownloadFinish,dw);

        return dw;

    }


    inline void AddDownloadTask(QString Url, QString path, QString Username = "", QString Password = "", int timeout = -1)
    {
        DownloadTask * task = new DownloadTask(Url,path,Username,Password,timeout);
        if(task->CanBeSaved())
        {
            DownloadWidget * widget = AddDownloadTaskToList(task);

            m_DownloadManager->AddDownload(task);
        }else {

            QMessageBox msgBox;
            msgBox.setText("This location can not be used to save a file: " + path);
            msgBox.exec();
        }

    }

    inline void AS_AddDownload()
    {

    }

    inline void AS_WaitForDownloads()
    {

    }

    inline void RegisterScriptFunctions()
    {


    }

public slots:


    inline void OnAddDownload(bool)
    {
        DownloadDialog * dialog = new DownloadDialog(m_SearchEdit->text());
        int res = dialog->exec();

        if(res = QDialog::Accepted)
        {
            AddDownloadTask(dialog->m_Url,dialog->m_File,dialog->m_Username,dialog->m_Password,dialog->do_time? dialog->time : -1);
        }else {

        }

        delete dialog;
        dialog = nullptr;
    }

    inline void OnSearchChanged(QString t)
    {
        if(t.isEmpty())
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                m_DownloadList->setItemHidden(m_ListItems.at(i),false);
            }
        }else
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                bool find = m_SearchText.at(i).contains(t,Qt::CaseSensitivity::CaseInsensitive);
                m_DownloadList->setItemHidden(m_ListItems.at(i),!find);
            }
        }
    }

};


#endif // DOWNLOADTOOL_H
