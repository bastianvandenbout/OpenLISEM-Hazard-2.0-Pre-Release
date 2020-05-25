#pragma once


#include "QWidget"
#include "QApplication"
#include "QMenuBar"
#include "QVBoxLayout"
#include "QTimer"

#define NODE_EDITOR_STATIC

#include "scriptmanager.h"
#include "scripttool.h"
#include "codeeditor.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>




class VisualScriptingTool : public QWidget
{
        Q_OBJECT;

public:

    QMutex m_LineInfoMutex;
    bool is_compilesucceed = false;
    bool is_compilechanged = false;
    QList<QString> m_Codes;
    QList<QtNodes::Node*> m_CodeLineNodes;
    int m_CurrentLine = -1; //-1 indicates nothing to highlight

    QTimer * m_Timer = nullptr;
    QWidget *popup;
    QLabel *popupLabel;
    QString popupMessage;



    QString m_Dir;

    QWaitCondition m_PauseWaitCondition;
    QMutex m_PauseMutex;

    std::atomic<bool> m_CodeIsRunning;
    std::atomic<bool> m_CodeIsPaused;
    std::atomic<bool> m_CodeIsStopRequested;
    std::atomic<bool> m_CodeIsPauseRequested;


    ScriptManager * m_ScriptManager;
    CodeEditor * m_ScriptTool;

    QString m_Path_Current;
    QString m_Path_OpenFiles;

    QtNodes::FlowView *m_FlowView;
    QtNodes::FlowScene *m_Scene;

    QMenuBar * menuBar;
    QAction *  compileAction;
    QAction * runAction;
    QAction * pauseAction;
    QAction * stopAction;
    QAction * saveAction;
    QAction * saveasAction;
    QAction * loadAction;
    QAction * newAction;

    QLabel * m_LabelRunning;

    VisualScriptingTool( ScriptManager * sm,QWidget * parent = nullptr);

    inline ~VisualScriptingTool()
    {

    }


    QList<QString> ConvertToScript();

    inline void int_emitupdatebuttons_start()
    {
        emit int_update_buttons_start();
    }
    inline void int_emitupdatebuttons_pause()
    {
        emit int_update_buttons_pause();
    }
    inline void int_emitupdatebuttons_stop()
    {
        emit int_update_buttons_stop();
    }

    inline void setCurrentLine(int l)
    {
        emit int_update_nodedone(l);
    }
signals:

    inline void int_update_nodedone(int);

    inline void int_update_buttons_start();
    inline void int_update_buttons_pause();
    inline void int_update_buttons_stop();

    inline void int_emit_show_dialog();
    inline void int_emit_show_temporary_dialog();
    inline void int_emit_hide_dialog();

public slots:

    void OnNodeDone(int l);

    inline void OnNodePlaced(QtNodes::Node & n)
    {
        m_LineInfoMutex.lock();
        m_Codes.clear();
        m_CodeLineNodes.clear();
        is_compilechanged = true;
        m_LineInfoMutex.unlock();
    }

    inline void OnNodeDeleted(QtNodes::Node & n)
    {
        m_LineInfoMutex.lock();
        m_Codes.clear();
        m_CodeLineNodes.clear();
        is_compilechanged = true;
        m_LineInfoMutex.unlock();
    }

    inline void connectionCreated(QtNodes::Connection const & c)
    {
        m_LineInfoMutex.lock();
        m_Codes.clear();
        m_CodeLineNodes.clear();
        is_compilechanged = true;
        m_LineInfoMutex.unlock();
    }

    inline void connectionDeleted(QtNodes::Connection const & c)
    {
        m_LineInfoMutex.lock();
        m_Codes.clear();
        m_CodeLineNodes.clear();
        is_compilechanged = true;
        m_LineInfoMutex.unlock();
    }


    inline void int_show_dialog()
    {
        popupLabel->setText(popupMessage);
        popup->adjustSize();
        popup->setVisible(true);
    }

    inline void int_show_temporary_dialog()
    {
         if(m_Timer != nullptr)
         {
             delete m_Timer;
             m_Timer  = nullptr;
         }
         m_Timer = new QTimer();
         m_Timer->start(3500);
         QObject::connect(m_Timer, SIGNAL(timeout()), this, SLOT(int_show_temporary_dialog_ontimer()));
         popupLabel->setText(popupMessage);
         popup->adjustSize();
         popup->setVisible(true);
    }

    inline void int_show_temporary_dialog_ontimer()
    {
        if(m_Timer != nullptr)
        {
            delete m_Timer;
            m_Timer = nullptr;
        }

        popup->setVisible(false);
    }

    inline void int_hide_dialog()
    {
        popup->setVisible(false);
    }

    inline void int_buttons_onpause()
    {
        runAction->setEnabled(true);
        pauseAction->setEnabled(false);
        stopAction->setEnabled(true);
        m_LabelRunning->setVisible(true);
    }
    inline void int_buttons_onstop()
    {
        runAction->setEnabled(true);
        pauseAction->setEnabled(false);
        stopAction->setEnabled(false);
        m_LabelRunning->setVisible(false);
    }
    inline void int_buttons_onstart()
    {
        runAction->setEnabled(false);
        pauseAction->setEnabled(true);
        stopAction->setEnabled(true);
        m_LabelRunning->setVisible(true);
    }

public slots:

    void OnSave(bool);
    void OnSaveas(bool);
    void OnLoad(bool);
    void OnNew(bool);
    void OnRun(bool);
    void OnPause(bool);
    void OnStop(bool);
    void OnCompile(bool);


};
