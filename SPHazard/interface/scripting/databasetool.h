#ifndef DATABASETOOL_H
#define DATABASETOOL_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include "functional"
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "codeeditor.h"
#include "QLineEdit"
#include "QIcon"
#include "iomanager.h"
#include "QCoreApplication"
#include "QToolButton"
#include "QComboBox"
#include "scripttool.h"
#include <QSplitter>
#include <iostream>
#include <QTimer>
#include <iostream>
#include <QFileIconProvider>
#include "layerinfowidget.h"
#include "elidedlabel.h"


class SPHFileSystemModel : public QFileSystemModel
{

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_delete;
    QIcon *icon_info;
    QIcon *icon_map;
    QIcon *icon_script;
    QIcon *icon_runfile;
    QIcon *icon_diropen;
    QIcon *icon_vector;
    QIcon *icon_pointcloud;
    QIcon *icon_table;

    QString m_Dir;
public:

    inline SPHFileSystemModel() : QFileSystemModel()
    {
        m_Dir = QCoreApplication::applicationDirPath();

        icon_start = new QIcon();
        icon_pause = new QIcon();
        icon_stop = new QIcon();
        icon_open = new QIcon();
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_new = new QIcon();
        icon_info = new QIcon();
        icon_delete = new QIcon();
        icon_map = new QIcon();
        icon_script = new QIcon();
        icon_runfile = new QIcon();
        icon_diropen = new QIcon();
        icon_table = new QIcon();
        icon_vector = new QIcon();
        icon_pointcloud = new QIcon();

        icon_start->addFile((m_Dir + SPH_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pause->addFile((m_Dir + SPH_FOLDER_ASSETS + "pause2.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_stop->addFile((m_Dir + SPH_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_open->addFile((m_Dir + SPH_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + SPH_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + SPH_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_new->addFile((m_Dir + SPH_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_info->addFile((m_Dir + SPH_FOLDER_ASSETS + "Info.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_delete->addFile((m_Dir + SPH_FOLDER_ASSETS + "deletereport4.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_map->addFile((m_Dir + SPH_FOLDER_ASSETS + "map.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_script->addFile((m_Dir + SPH_FOLDER_ASSETS + "onewfile.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_runfile->addFile((m_Dir + SPH_FOLDER_ASSETS + "iconfinder_Report_132648.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_diropen->addFile((m_Dir + SPH_FOLDER_ASSETS + "diropen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_table->addFile((m_Dir + SPH_FOLDER_ASSETS + "table.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_vector->addFile((m_Dir + SPH_FOLDER_ASSETS + "vector.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pointcloud->addFile((m_Dir + SPH_FOLDER_ASSETS + "pointcloud.png"), QSize(), QIcon::Normal, QIcon::Off);
    }

    inline QVariant data( const QModelIndex& index, int role ) const override
    {

        if( role == Qt::DecorationRole && index.column() == 0 )
        {
            QFileInfo info = SPHFileSystemModel::fileInfo(index);

            if(info.isFile())
            {
                if(info.suffix() == "map" || info.suffix() == "asc" || info.suffix() == "tif" )
                {
                    return *icon_map;//I pick the icon depending on the extension
                }else if(info.suffix() == "run")
                {
                    return *icon_runfile;
                }
                else if(info.suffix() == "script")
                {
                    return *icon_script;
                }else if(info.suffix() == "shp")
                {
                    return *icon_vector;
                }else if(info.suffix() == "las")
                {
                    return *icon_pointcloud;
                }else if(info.suffix() == "csv" ||info.suffix() == "tbl")
                {
                    return *icon_table;
                }
            }
        }
        return QFileSystemModel::data(index, role);
    }


};


class SPHLineEdit : public QPlainTextEdit
{
        Q_OBJECT;
public:


    QList<QString> m_CommandHistory;
    int m_CommandHistoryIndex = -1;
    bool m_AllowCommands = true;

    ScriptManager * m_ScriptManager;

    SPHScriptCompleter * m_Completer = nullptr;


    inline SPHLineEdit(ScriptManager * s, QWidget * p) : QPlainTextEdit(p)
    {

        m_ScriptManager = s;

        QFontMetrics metrics(font());
        int lineHeight = metrics.lineSpacing();
        setFixedHeight(std::max(30,lineHeight));

        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setLineWrapMode(QPlainTextEdit::NoWrap);

        if(m_ScriptManager != nullptr)
        {
            QStringList functionnames = m_ScriptManager->GetGlobalFunctionNames();

            QList<ScriptFunctionInfo> funclist = m_ScriptManager->m_Engine->GetGlobalFunctionList();
            QStringList fnames;
            QStringList fdescrp;

            for(int i = 0; i < funclist.size(); i++)
            {
                fnames.append(funclist.at(i).Function->GetName());
                QString full = m_ScriptManager->m_Engine->GetTypeName(funclist.at(i).Function->GetReturnTypeId()) + " " +funclist.at(i).Function->GetName();
                full.append("(");
                //if(funclist.at(i).Parameters.size() > 0)
                {
                    for(int j = 0; j < funclist.at(i).Function->GetParamCount(); j++)
                    {
                        int id = 0;
                        funclist.at(i).Function->GetParam(j,&id);
                        full.append(m_ScriptManager->m_Engine->GetTypeName(id) + " ");
                        if(funclist.at(i).Parameters.size() > j)
                        {
                            full.append(funclist.at(i).Parameters.at(j));
                        }

                        if(j != funclist.at(i).Function->GetParamCount()-1)
                        {
                            full.append(",");
                        }
                    }

                }
                full.append(")");
                full.append("\n");
                full.append("\n");
                full.append(funclist.at(i).Description);

                fdescrp.append(full);

            }
            m_Completer = new SPHScriptCompleter(fnames,fdescrp,nullptr);
            m_Completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
            m_Completer->setCaseSensitivity(Qt::CaseInsensitive);
            m_Completer->setCompletionMode(QCompleter::PopupCompletion);
            m_Completer->setWrapAround(true);
            this->setCompleter(m_Completer);

        }


        connect(this,SIGNAL(returnPressed()),this,SLOT(OnConsoleEnterPressed()));
    }

    inline void setCompleter(SPHScriptCompleter *completer)
    {
        if (m_Completer)
        {
            QObject::disconnect(m_Completer, 0, this, 0);
        }

        m_Completer = completer;

        if (!m_Completer)
            return;

        m_Completer->setWidget(this);
        m_Completer->setCompletionMode(QCompleter::PopupCompletion);
        m_Completer->setCaseSensitivity(Qt::CaseInsensitive);
        QObject::connect(m_Completer, QOverload<const QString &>::of(&QCompleter::activated),
                         this, &SPHLineEdit::insertCompletion);
    }

    void insertCompletion(const QString& completion)
    {
        if (m_Completer->widget() != this)
        {
            return;
        }

        QTextCursor tc = textCursor();
        int extra = completion.length() - m_Completer->completionPrefix().length();
        tc.movePosition(QTextCursor::Left);
        tc.movePosition(QTextCursor::EndOfWord);
        tc.insertText(completion.right(extra));
        setTextCursor(tc);
    }

    inline void SetDir(QString dir)
    {
        if(m_Completer != nullptr)
        {
            m_Completer->SetDirectory(dir);
        }
    }

    QString textUnderCursor() const
    {
        QTextCursor tc = textCursor();
        tc.select(QTextCursor::WordUnderCursor);
        return tc.selectedText();
    }

    inline void keyPressEvent(QKeyEvent *event) override
    {

        if (m_Completer && m_Completer->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
           switch (event->key()) {
           case Qt::Key_Enter:
           case Qt::Key_Return:
           case Qt::Key_Escape:
           case Qt::Key_Tab:
           case Qt::Key_Backtab:
                event->ignore();
                return; // let the completer do default behavior
           default:
               break;
           }
        }


        bool isShortcut = ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E); // CTRL+E

        if(!m_Completer || !isShortcut)
        {

            if((event->key() == Qt::Key_Up) || (event->key() == Qt::Key_Down))
            {
                int m_CommandHistoryIndexO =  m_CommandHistoryIndex;

                if((event->key() == Qt::Key_Up) )
                {

                     m_CommandHistoryIndex += 1;
                }
                else if(event->key() == Qt::Key_Down)
                {
                    m_CommandHistoryIndex -= 1;
                }


                //limit to -1 (new command) and length of the history
                 m_CommandHistoryIndex = std::min(m_CommandHistory.length()-1,std::max(-1, m_CommandHistoryIndex));

                 //if there is a difference in index, we know we must change the text
                 if(m_CommandHistoryIndexO != m_CommandHistoryIndex)
                 {
                       if(m_CommandHistoryIndex == -1)
                       {
                            this->document()->setPlainText("");
                       }else
                       {
                            this->document()->setPlainText(m_CommandHistory.at(m_CommandHistoryIndex));
                       }

                 }




            }else {

                if( (event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return))
                {
                    OnConsoleEnterPressed();
                }else
                {
                    QPlainTextEdit::keyPressEvent(event);
                }

            }
        }



        const bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
        if (!m_Completer || (ctrlOrShift && event->text().isEmpty()))
        {
            return;
        }

        static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
        bool hasModifier = (event->modifiers() != Qt::NoModifier) && !ctrlOrShift;
        QString completionPrefix = textUnderCursor();

        if (!isShortcut && (hasModifier || event->text().isEmpty()|| completionPrefix.length() < 3
                          || eow.contains(event->text().right(1)))) {

            m_Completer->popup()->hide();
            return;
        }


        m_Completer->update(completionPrefix);
        //if (completionPrefix != m_Completer->completionPrefix()) {

            m_Completer->popup()->setCurrentIndex(m_Completer->completionModel()->index(0, 0));
        //}
        QRect cr = cursorRect();
        cr.setWidth(m_Completer->popup()->sizeHintForColumn(0)
                    + m_Completer->popup()->verticalScrollBar()->sizeHint().width());



        m_Completer->complete(cr); // popup it up!


        m_Completer->ShowPopup0();

    }

    inline void AllowCommands(bool x)
    {
        m_AllowCommands = x;
    }

public:
signals:

    void OnCommandGiven(QString c);


public slots:

    inline void OnConsoleEnterPressed()
    {
        if(m_AllowCommands)
        {

            QString command = this->document()->toRawText();

            if(!command.isEmpty())
            {

                m_CommandHistory.prepend(command);
                m_CommandHistoryIndex = -1;
                OnCommandGiven(command);

            }
        }

    }



};


class DatabaseTool : public QWidget
{
        Q_OBJECT;

public:

    QMutex * m_ConsoleMutex;
    bool m_ConsoleRunning = false;

    QSplitter *m_MainWidget;
    QPlainTextEdit *m_FileConsole;
    SPHLineEdit * m_FileConsoleLineEdit;
    ScriptTool * m_FileEditor;
    QTabWidget * m_TabFiles;
    QWidget * m_MenuWidget;
    QHBoxLayout * m_MenuLayout;
    QComboBox * m_DirBox;

    QString m_Dir;

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_delete;
    QIcon *icon_info;
    QIcon *icon_map;
    QIcon *icon_script;
    QIcon *icon_runfile;
    QIcon *icon_diropen;

    ScriptManager *m_ScriptManager;

    QFileSystemModel *model;
    QTreeView *m_BrowseTree;
    SPHFileSystemModel *model2;
    QTreeView *m_BrowseTree2;

    ElidedLabel * m_DirLabel;

    QString m_Path_Work;
    QString m_Path_Current;
    QString m_Path_OpenFiles;

    inline DatabaseTool( ScriptManager * sm,QWidget *parent = 0, const char *name = 0 ): QWidget( parent)
    {

        m_ScriptManager = sm;
        m_Dir = QCoreApplication::applicationDirPath();

        m_ConsoleMutex = new QMutex();

        icon_start = new QIcon();
        icon_pause = new QIcon();
        icon_stop = new QIcon();
        icon_open = new QIcon();
        icon_save = new QIcon();
        icon_saveas = new QIcon();
        icon_new = new QIcon();
        icon_info = new QIcon();
        icon_delete = new QIcon();
        icon_map = new QIcon();
        icon_script = new QIcon();
        icon_runfile = new QIcon();
        icon_diropen = new QIcon();

        icon_start->addFile((m_Dir + SPH_FOLDER_ASSETS + "start1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_pause->addFile((m_Dir + SPH_FOLDER_ASSETS + "pause2.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_stop->addFile((m_Dir + SPH_FOLDER_ASSETS + "stop1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_open->addFile((m_Dir + SPH_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_save->addFile((m_Dir + SPH_FOLDER_ASSETS + "filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_saveas->addFile((m_Dir + SPH_FOLDER_ASSETS + "filesaveas.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_new->addFile((m_Dir + SPH_FOLDER_ASSETS + "new.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_info->addFile((m_Dir + SPH_FOLDER_ASSETS + "Info.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_delete->addFile((m_Dir + SPH_FOLDER_ASSETS + "deletereport4.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_map->addFile((m_Dir + SPH_FOLDER_ASSETS + "map.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_script->addFile((m_Dir + SPH_FOLDER_ASSETS + "onewfile.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_runfile->addFile((m_Dir + SPH_FOLDER_ASSETS + "iconfinder_Report_132648.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon_diropen->addFile((m_Dir + SPH_FOLDER_ASSETS + "diropen.png"), QSize(), QIcon::Normal, QIcon::Off);


        model = new QFileSystemModel();
        model->setRootPath("");
        model->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QFileIconProvider * iconp =  model->iconProvider();
        m_BrowseTree = new QTreeView();
        QString rootPath = "";//QCoreApplication::applicationDirPath();
        m_BrowseTree->setModel(model);
           if (!rootPath.isEmpty()) {
                const QModelIndex rootIndex = model->index(QDir::cleanPath(rootPath));
                if (rootIndex.isValid())
                    m_BrowseTree->setRootIndex(rootIndex);
            }


       m_BrowseTree->setAnimated(false);
       m_BrowseTree->setIndentation(20);
       m_BrowseTree->setSortingEnabled(true);
       m_BrowseTree->hideColumn(1);
       m_BrowseTree->hideColumn(2);
       m_BrowseTree->hideColumn(3);
       m_BrowseTree->setDropIndicatorShown(true);
       m_BrowseTree->setAcceptDrops(true);
       m_BrowseTree->viewport()->setAcceptDrops(true);

       model2 = new SPHFileSystemModel();
       model2->setRootPath("C:/");
       model2->setFilter(QDir::Files | QDir::NoDotAndDotDot);

       QFileIconProvider * iconp2 =  model->iconProvider();

       m_BrowseTree2 = new QTreeView();
       m_BrowseTree2->setDragEnabled(true);
       m_BrowseTree2->setDropIndicatorShown(true);
       m_BrowseTree2->setAcceptDrops(true);
       m_BrowseTree2->setColumnWidth(0,300);
       m_BrowseTree2->viewport()->setAcceptDrops(true);
       QString rootPath2 = QCoreApplication::applicationDirPath();
       m_BrowseTree2->setModel(model2);
          if (!rootPath2.isEmpty()) {
               const QModelIndex rootIndex2 = model2->index(QDir::cleanPath(rootPath));
               if (rootIndex2.isValid())
                   m_BrowseTree2->setRootIndex(rootIndex2);
           }


      m_BrowseTree2->setAnimated(false);
      m_BrowseTree2->setIndentation(20);
      m_BrowseTree2->setSortingEnabled(true);


     m_FileConsole = new QPlainTextEdit(this);
     m_FileConsoleLineEdit = new SPHLineEdit(m_ScriptManager,this);

     m_TabFiles = new QTabWidget();
     m_TabFiles->setTabsClosable(true);
     m_TabFiles->setMovable(true);

     m_FileEditor = new ScriptTool(sm);


     m_MenuWidget = new QWidget(this);
     m_MenuLayout = new QHBoxLayout(m_MenuWidget);
     m_MenuLayout->setMargin(0);


     QToolButton *DeleteButton = new QToolButton(this);
     DeleteButton->setIcon(*icon_delete);
     DeleteButton->setIconSize(QSize(22,22));
     DeleteButton->setMaximumSize(QSize(22,22));
     DeleteButton->resize(22,22);
     DeleteButton->setEnabled(true);

     QToolButton *OpenButton = new QToolButton(this);
     OpenButton->setIcon(*icon_open);
     OpenButton->setIconSize(QSize(22,22));
     OpenButton->setMaximumSize(QSize(22,22));
     OpenButton->resize(22,22);
     OpenButton->setEnabled(true);

     QToolButton *DisplayButton = new QToolButton(this);
     DisplayButton->setIcon(*icon_map);
     DisplayButton->setIconSize(QSize(22,22));
     DisplayButton->setMaximumSize(QSize(22,22));
     DisplayButton->resize(22,22);
     DisplayButton->setEnabled(true);

     QToolButton *OpenScriptButton = new QToolButton(this);
     OpenScriptButton->setIcon(*icon_script);
     OpenScriptButton->setIconSize(QSize(22,22));
     OpenScriptButton->setMaximumSize(QSize(22,22));
     OpenScriptButton->resize(22,22);
     OpenScriptButton->setEnabled(true);

     QToolButton *OpenRunfileButton = new QToolButton(this);
     OpenRunfileButton->setIcon(*icon_runfile);
     OpenRunfileButton->setIconSize(QSize(22,22));
     OpenRunfileButton->setMaximumSize(QSize(22,22));
     OpenRunfileButton->resize(22,22);
     OpenRunfileButton->setEnabled(true);

     QToolButton *InfoButton = new QToolButton(this);
     InfoButton->setIcon(*icon_info);
     InfoButton->setIconSize(QSize(22,22));
     InfoButton->setMaximumSize(QSize(22,22));
     InfoButton->resize(22,22);
     InfoButton->setEnabled(true);

     QToolButton *DirOpenButton = new QToolButton(this);
     DirOpenButton->setIcon(*icon_diropen);
     DirOpenButton->setIconSize(QSize(22,22));
     DirOpenButton->setMaximumSize(QSize(22,22));
     DirOpenButton->resize(22,22);
     DirOpenButton->setEnabled(true);



     m_DirLabel = new ElidedLabel();

     m_MenuLayout->setMargin(0);
     m_MenuLayout->addWidget(DeleteButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(OpenButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(DisplayButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(OpenScriptButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(OpenRunfileButton,0,Qt::AlignLeft);
     m_MenuLayout->addWidget(InfoButton,0,Qt::AlignLeft);
     m_MenuLayout->addItem(new QSpacerItem(20,20));
     m_MenuLayout->addWidget(DirOpenButton);
     m_MenuLayout->addWidget(m_DirLabel);

     m_MenuLayout->addStretch();

     QWidget * m_BrowseWidget = new QWidget();
     QHBoxLayout * m_BrowseHLayout = new QHBoxLayout();
     m_BrowseWidget->setLayout(m_BrowseHLayout);
     QWidget * mapVtab = new QWidget();
     QHBoxLayout *maplayout = new QHBoxLayout();
     QVBoxLayout *mapvlayout = new QVBoxLayout();

     m_MainWidget = new QSplitter();
     this->setLayout(maplayout);
     maplayout->addWidget(m_MainWidget);

     m_MainWidget->addWidget(m_FileEditor);
     m_MainWidget->setCollapsible(m_MainWidget->indexOf(m_FileEditor),false);
     m_MainWidget->setStretchFactor(m_MainWidget->indexOf(m_FileEditor),3);

     m_MainWidget->addWidget(mapVtab);
     m_MainWidget->setCollapsible(m_MainWidget->indexOf(mapVtab),false);
     m_MainWidget->setStretchFactor(m_MainWidget->indexOf(mapVtab),1);
     mapVtab->setLayout(mapvlayout);
     mapvlayout->addWidget(m_BrowseWidget);
     m_BrowseHLayout->addWidget(m_BrowseTree);
     m_BrowseHLayout->addWidget(m_BrowseTree2);
     mapvlayout->addWidget(m_MenuWidget);
     mapvlayout->addWidget(m_FileConsole);
     mapvlayout->addWidget(m_FileConsoleLineEdit);

     connect(m_BrowseTree,SIGNAL(clicked(const QModelIndex &)),this, SLOT(OnFileModelClicked(const QModelIndex &)));

     connect(m_BrowseTree2,SIGNAL(clicked(const QModelIndex &)),this, SLOT(OnFileModel2Clicked(const QModelIndex &)));
     connect(m_BrowseTree2,SIGNAL(doubleClicked(const QModelIndex &)),this, SLOT(OnFileModel2DClicked(const QModelIndex &)));

     connect(DeleteButton,SIGNAL(pressed()),this,SLOT(OnDelete()));
     connect(OpenButton,SIGNAL(pressed()),this,SLOT(OnOpen()));
     connect(DisplayButton,SIGNAL(pressed()),this,SLOT(OnDisplay()));
     connect(OpenScriptButton,SIGNAL(pressed()),this,SLOT(OnOpenScript()));
     connect(OpenRunfileButton,SIGNAL(pressed()),this,SLOT(OnOpenRunFile()));
     connect(InfoButton,SIGNAL(pressed()),this,SLOT(OnInfo()));
     connect(DirOpenButton,SIGNAL(pressed()),this,SLOT(OnDirOpen()));

     connect(m_FileConsoleLineEdit,SIGNAL(OnCommandGiven(QString)),this,SLOT(OnConsoleCommand(QString)));
     QTimer::singleShot(0,this,SLOT(UpdateConsole()));

     m_Path_Current = QCoreApplication::applicationDirPath()+"/";
     m_Path_OpenFiles = m_Path_Current + "openloc.ini";


     QFile fin(m_Path_OpenFiles);
     if (!fin.open(QFile::ReadOnly | QFile::Text)) {

         if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate |QIODevice::Text))
         {
             QTextStream out(&fin);
             out << QString("[Open Location]\n");
         }
         fin.close();
     }else
     {
         while (!fin.atEnd())
         {
             QString S = fin.readLine().trimmed();
             QFile fin_temp(S);
             if(!S.startsWith("["))
             {
                 if (QDir(S).exists())
                 {
                     QModelIndex index = model->index(QDir::cleanPath(S));
                     m_BrowseTree->setCurrentIndex(index);
                     SetRootFolder(S);
                     m_Path_Work = S;

                     m_DirLabel->setText(S);
                     m_FileEditor->SetHomeDir(S);
                     m_FileConsoleLineEdit->SetDir(S);
                 }
             }
         }
     }

     ExportOpenLoc();

    }

    inline void ExportOpenLoc()
    {
        QFile fin(m_Path_OpenFiles);
        if (fin.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&fin);
            out << QString("[Runfile List]\n");

            out << m_DirLabel->text() << "\n";

        }
        fin.close();

    }

    inline ~DatabaseTool()
    {

        delete m_ConsoleMutex;
    }

    std::function<bool(QString, int)> m_FileCallBack;
    bool m_FileCallBackSet = false;

    template<typename _Callable, typename... _Args>
    inline void SetCallBackFileOpened(_Callable&& __f, _Args&&... __args)
    {
        //now we store this function pointer and call it later when a file is openened
        m_FileCallBack = std::bind(std::forward<_Callable>(__f),std::forward<_Args>(__args)...,std::placeholders::_1,std::placeholders::_2);
        m_FileCallBackSet = true;
    }

public slots:

    void UpdateConsole()
    {
        QList<LeveledMessage> list = GetMessagesS();

        for(int i = 0; i < list.length(); i++)
        {
            QString line = list.at(i).Message;

            QString alertHtml = "<font color=\"DeepPink\">";
            QString notifyHtml = "<font color=#404040>";
            QString infoHtml = "<font color=#003399>";
            QString errorHtml = "<font color=\"Red\">";
            QString scriptHtml = "<font color=\"Indigo\">";
            QString endHtml = "</font>";

            switch(list.at(i).Level)
            {
                case SPH_MESSAGE_ERROR: line = errorHtml.append(line); break;
                case SPH_MESSAGE_WARNING: line = alertHtml.append(line); break;
                case SPH_MESSAGE_STATUS: line = notifyHtml.append(line); break;
                case SPH_MESSAGE_DEBUG: line = infoHtml.append(line); break;
                case SPH_MESSAGE_SCRIPT: line = infoHtml.append(line); break;
                default: line = notifyHtml.append(line); break;
            }

            line = line.append(endHtml);
            m_FileConsole->appendHtml(line);
        }

        QTimer::singleShot(0.01,this,SLOT(UpdateConsole()));

    }

    inline void OnFileModelClicked(const QModelIndex &index)
    {
        SetRootFolder(model->filePath(index));
    }

    inline void SetRootFolder(QString rootPath2)
    {
        delete model2;
        model2 = new SPHFileSystemModel();
        model2->setFilter(QDir::Files | QDir::NoDotAndDotDot);
        QFileIconProvider * iconp2 =  model->iconProvider();

        model2->setRootPath(rootPath2);
        m_BrowseTree2->setModel(model2);

        if (!rootPath2.isEmpty()) {
             const QModelIndex rootIndex2 = model2->index(QDir::cleanPath(rootPath2));
             if (rootIndex2.isValid())

                 m_BrowseTree2->setRootIndex(rootIndex2);
         }

        m_BrowseTree2->update();
        m_BrowseTree2->setColumnWidth(0,300);

    }

    inline void OnFileModel2DClicked(const QModelIndex &index)
    {
        QString filepath = model->filePath(index);

        QFileInfo f = QFileInfo(filepath);
        if(f.exists())
        {
            //check extension to determine automatic usage of the file
            //if nothing found, use the default os file opening routine

            if(filepath.length() > 3)
            {
                QString ext = filepath.right(4);

                if((ext.compare(SPH_FILE_EXT_SCRIPT) == 0) || (ext.compare(SPH_FILE_EXT_RUN) == 0))
                {
                    //raster extension, try to open with map display
                    m_FileEditor->OpenCode(filepath);

                }else if(ext.compare(".tbl") == 0 || ext.compare(".csv") == 0)
                {
                    //table extension
                    MatrixTable * t = new MatrixTable();
                    t->LoadFromFileCSV(filepath);

                    LayerInfoWidget *w = new LayerInfoWidget(t);
                    w->show();

                }
            }
            if(m_FileCallBackSet)
            {
                bool used = m_FileCallBack(filepath, SPH_FILE_TYPE_UNKNOWN);
            }

        }else {

            SPHS_ERROR("Could not open file: " +filepath);
        }


    }

    inline void OnFileModel2Clicked(const QModelIndex &index)
    {





    }

    inline void OnDelete()
    {
        QModelIndex index = m_BrowseTree2->currentIndex();
        QString file = model2->filePath(index);

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete", "Delete this file?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {

            QFile(file);
            if(file.exists())
            {
                file.remove();
            }
        } else {


        }

    }

    inline void OnDirOpen()
    {
        m_Path_Work =model->filePath(m_BrowseTree->currentIndex());
        m_DirLabel->setText(m_Path_Work);

        ExportOpenLoc();

        m_FileEditor->SetHomeDir(model->filePath(m_BrowseTree->currentIndex()));
        m_FileConsoleLineEdit->SetDir(model->filePath(m_BrowseTree->currentIndex()));

    }

    inline void OnOpen()
    {


    }

    inline void OnOpenScript()
    {


    }

    inline void OnDisplay()
    {
        QModelIndex index = m_BrowseTree2->currentIndex();
        QString file = model2->filePath(index);
        if(m_FileCallBackSet)
        {
            bool used = m_FileCallBack(file, SPH_FILE_TYPE_MAP);
        }


    }

    inline void OnOpenRunFile()
    {
        QModelIndex index = m_BrowseTree2->currentIndex();
        QString file = model2->filePath(index);



    }

    inline void OnInfo()
    {


    }

    inline void OnScriptDone()
    {
        SPHS_DEBUG("Done");
    }

    inline void OnScriptCallBackPrint(QString s)
    {

    }

    inline void OnConsoleCommand(QString command)
    {
        m_ConsoleMutex->lock();
        m_ConsoleRunning = true;
        m_FileConsoleLineEdit->AllowCommands(false);
        m_ConsoleMutex->unlock();

        m_FileConsole->appendPlainText(command);
        m_FileConsoleLineEdit->clear();

        SPHScript *s = new SPHScript();
        s->SetCode(command);
        s->SetSingleLine(true);
        s->SetPreProcess(true);
        s->SetHomeDir(m_Path_Work+"/");

        s->SetCallBackPrint(std::function<void(DatabaseTool *,SPHScript*,QString)>([](DatabaseTool *,SPHScript*,QString) ->
                                                                    void{


            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackDone(std::function<void(DatabaseTool *,SPHScript*,bool x)>([](DatabaseTool *dt,SPHScript*, bool finished) ->
                                                                    void{

            dt->m_ConsoleMutex->lock();
            dt->m_FileConsoleLineEdit->AllowCommands(true);
            dt->m_ConsoleRunning = true;
            dt->m_ConsoleMutex->unlock();
            SPHS_DEBUG("Done");
            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackCompilerError(std::function<void(DatabaseTool *,SPHScript*,const asSMessageInfo *msg)>([](DatabaseTool *,SPHScript*,const asSMessageInfo *msg) ->
                                                                    void{
            SPHS_ERROR("Error in script execution");
            const char *type = "ERR ";
            if( msg->type == asMSGTYPE_WARNING )
            {
                type = "WARN";
            }
            else if( msg->type == asMSGTYPE_INFORMATION )
            {
                type = "INFO";
            }
            SPHS_ERROR(QString(msg->section) + " (" + QString(msg->row) + ", " + QString(msg->col) + ") : " + QString(type) + " : " + QString(msg->message));
            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackException(std::function<void(DatabaseTool *,SPHScript*,asIScriptContext *ctx)>([](DatabaseTool *,SPHScript*,asIScriptContext *ctx) ->
                                                                    void{

            SPHS_ERROR("Exception encountered when running script");

            ;
                                                                    }),this,s,std::placeholders::_1);

        s->SetCallBackLine(std::function<void(DatabaseTool *,SPHScript*,asIScriptContext *ctx)>([](DatabaseTool *,SPHScript*,asIScriptContext *ctx) ->
                           void{


            ;
                           }),this,s,std::placeholders::_1);

        m_ScriptManager->CompileScript_Generic(s);
        if(s->IsCompiled())
        {
            m_ScriptManager->RunScript(s);
        }else
        {
            SPHS_STATUS("Compilation Error");
            m_ConsoleMutex->lock();
            m_ConsoleRunning = true;
            m_FileConsoleLineEdit->AllowCommands(true);
            m_ConsoleMutex->unlock();
        }
    }


};

#endif // DATABASETOOL_H
