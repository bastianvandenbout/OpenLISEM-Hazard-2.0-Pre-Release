#ifndef QTINTERFACEWINDOW_H
#define QTINTERFACEWINDOW_H



#ifdef OS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#ifdef OS_LNX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_GLX
#endif


#include <glad/glad.h>
#include "OpenCLUtil.h"
#include "cl.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "OpenGLUtil.h"

#include <QObject>

#include <QApplication>
#include <QtWidgets>
#include <QSystemTrayIcon>
#include <QPlainTextEdit>
#include <QSystemTrayIcon>
#include "parametermanager.h"
#include "model.h"
#include "scriptmanager.h"
#include "worldwindow.h"

#include "codeeditor.h"
#include "scripttool.h"
#include "databasetool.h"
#include "mapviewtool.h"
#include "consoletool.h"
#include "modelstattool.h"
#include "modeltool.h"
#include "devicewidget.h"
#include "toolbrowser.h"
#include "downloadtool.h"
#include "visualscripting.h"

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QPalette>

class QTInterfaceWindow : public QMainWindow
{

    Q_OBJECT

public:

    QString m_Dir;

    WorldWindow * m_WorldWindow;
    ParameterManager * m_ParameterManager;
    SPHModel * m_Model;
    ScriptManager * m_ScriptManager;

    QWidget * m_MainWidget;
    QWidget * m_ParameterWidget;

    ModelTool * m_ModelTool;
    ModelStatTool * m_ModelStatTool;
    MapViewTool * m_MapViewTool;
    ConsoleTool * m_ConsoleTool;
    ScriptTool * m_ScriptWidget;
    DatabaseTool* m_DatabaseWidget;
    VisualScriptingTool * m_VisualScripter;
    ToolBrowser * m_ToolBoxWidget;
    DownloadManagerWidget * m_DownloadWidget;

    QMenu *fileMenu;
    QTabWidget * TabWidget;
    QComboBox * m_DropDown;
    QToolButton *m_delButton;
    QVBoxLayout *m_Layout;
    QList<QVBoxLayout *> TabWidgetLayouts;

    QProgressBar * m_ProgressBar;

    QSignalMapper *m_SignalMapper_UI;

    QSignalMapper *m_SignalMapper_Bool;
    QSignalMapper *m_SignalMapper_Map;
    QSignalMapper *m_SignalMapper_Dir;
    QSignalMapper *m_SignalMapper_File;
    QSignalMapper *m_SignalMapper_Float;
    QSignalMapper *m_SignalMapper_Int;
    QSignalMapper *m_SignalMapper_MultiCheck;

    QList<ParameterWidget> m_ParameterWidgetList;

    QIcon *icon_start;
    QIcon *icon_pause;
    QIcon *icon_stop;
    QIcon *icon_open;
    QIcon *icon_save;
    QIcon *icon_saveas;
    QIcon *icon_new;
    QIcon *icon_info;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveasAct;
    QAction *printAct;
    QAction *exitAct;

    QAction *startAct;
    QAction *pauseAct;
    QAction *stopAct;

    QString m_Path_Current;
    QString m_Path_runfiles;


    bool m_darkui = false;
public:
    QTInterfaceWindow(QWidget * w= NULL ) : QMainWindow(w)
    {


    }

    int Create(ParameterManager * pm, SPHModel * m, ScriptManager * sm, WorldWindow * w);


    inline void contextMenuEvent(QContextMenuEvent *event) override
    {
        //bad code, the QAction instance needs to be deleted

        /*QMenu menu(this);
        QAction *cInfoAct = new QAction("About",this);

        menu.addAction(cInfoAct);
        menu.exec(event->globalPos());*/
    }

    inline void SetScriptFunctions(ScriptManager * sm)
    {
        m_MapViewTool->SetScriptFunctions(sm);


    }

    void CreateParameterWidgets();

    void UpdateParameters();
    void UpdateInterface();


    MODELTOINTERFACE m_ModelData;

    inline void OnModelStepDone()
    {

        //this is the moment to exchange any data between the main interface and the model while it is running
        m_Model->m_ModelDataMutex.lock();



        m_Model->m_InterfaceData.m_CurrentUIMap = m_ModelData.m_CurrentUIMap;

        //from model to interface
        m_Model->m_InterfaceData.m_UIMapNames =m_Model->m_UIMapNames;
        m_Model->m_InterfaceData.val_max = m_ModelData.val_max;
        m_Model->m_InterfaceData.val_min = m_ModelData.val_min;

        m_ModelData = m_Model->m_InterfaceData;

        //for pointers we must make a duplicate manually
        if(m_ModelData.H.length() < m_ModelData.outlets.length())
        {
            for(int i = 0; i < m_ModelData.outlets.length(); i++)
            {
                m_ModelData.H.append(new QList<float>());
            }

        }
        if(m_ModelData.V.length() < m_ModelData.outlets.length())
        {
            for(int i = 0; i < m_ModelData.outlets.length(); i++)
            {
                m_ModelData.V.append(new QList<float>());
            }
        }
        if(m_ModelData.Q.length() < m_ModelData.outlets.length())
        {
            for(int i = 0; i < m_ModelData.outlets.length(); i++)
            {
                m_ModelData.Q.append(new QList<float>());
            }
        }
        for(int i = 0; i < m_ModelData.outlets.length(); i++)
        {
            m_ModelData.H.at(i)->fromStdList(m_Model->m_InterfaceData.H.at(i)->toStdList());
            m_ModelData.V.at(i)->fromStdList(m_Model->m_InterfaceData.V.at(i)->toStdList());
            m_ModelData.Q.at(i)->fromStdList(m_Model->m_InterfaceData.Q.at(i)->toStdList());
        }


        //from interface to model
        if(m_Model->m_InterfaceData.m_mapnameschanged == true)
        {
            m_Model->m_InterfaceData.m_mapnameschanged = false;
        }


        m_Model->m_ModelDataMutex.unlock();

        QTimer::singleShot(0,this,SLOT(UpdateInterfaceFromModelData()));

    }

    inline void closeEvent(QCloseEvent *event) override
    {
        m_WorldWindow->SignalClose();
        event->ignore();
    }

    bool OnFileOpenRequest(QString path, int type);

public slots:


    void UpdateInterfaceFromModelData();

    void newFile();
    void open();
    void save();
    void saveas();
    void ExportRunfile(bool saveas = false);
    void ImportRunfile(QString name= QString(""));
    void ExportRunfileList();
    void OnRunfileIndexChanged(int i);
    void OnRunFileDeleteFromList();
    void print();
    void cut();
    void copy();
    void paste();
    inline void SignalFunction_UIButton(QString arg)
    {


    }

    void start();
    void pause();
    void stop();
    void OnModelPaused();
    void OnModelStopped(bool);
    void ModelPaused();
    void ModelStopped(bool has_error);


    void SignalFunction_UI(QString);


    void SignalFunction_Bool(int index);
    void SignalFunction_Map(int index);
    void SignalFunction_Dir(int index);
    void SignalFunction_File(int index);
    void SignalFunction_Float(int index);
    void SignalFunction_Int(int index);
    void SignalFunction_MultiCheck(int index);


    inline void Device()
    {

        DeviceWidget *w = new DeviceWidget(m_WorldWindow->m_OpenGLCLManager);
        w->show();

    }

    inline void ToggleTheme()
    {
        if(!m_darkui)
        {
            // modify palette to dark
            QPalette darkPalette;
            darkPalette.setColor(QPalette::Window,QColor(53,53,53));
            darkPalette.setColor(QPalette::WindowText,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
            darkPalette.setColor(QPalette::Base,QColor(42,42,42));
            darkPalette.setColor(QPalette::AlternateBase,QColor(66,66,66));
            darkPalette.setColor(QPalette::ToolTipBase,Qt::white);
            darkPalette.setColor(QPalette::ToolTipText,Qt::white);
            darkPalette.setColor(QPalette::Text,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
            darkPalette.setColor(QPalette::Dark,QColor(35,35,35));
            darkPalette.setColor(QPalette::Shadow,QColor(20,20,20));
            darkPalette.setColor(QPalette::Button,QColor(53,53,53));
            darkPalette.setColor(QPalette::ButtonText,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
            darkPalette.setColor(QPalette::BrightText,Qt::red);
            darkPalette.setColor(QPalette::Link,QColor(42,130,218));
            darkPalette.setColor(QPalette::Highlight,QColor(42,130,218));
            darkPalette.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
            darkPalette.setColor(QPalette::HighlightedText,Qt::white);
            darkPalette.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

            qApp->setPalette(darkPalette);

        }else
        {
            qApp->setPalette(this->style()->standardPalette());
        }

        m_darkui = !m_darkui;
    }
};

#endif // QTINTERFACEWINDOW_H
