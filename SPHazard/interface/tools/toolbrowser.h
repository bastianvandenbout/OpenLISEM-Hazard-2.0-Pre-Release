#pragma once
#include "QWidget"
#include "scriptmanager.h"
#include "databasetool.h"
#include "mapviewtool.h"
#include "QLineEdit"
#include "QPushButton"
#include "QToolButton"
#include "QStringListModel"
#include "QItemDelegate"
#include "QLabel"
#include "QHBoxLayout"
#include "tooldialog.h"
#include "QFont"

class ToolWidget : public QWidget
{

        Q_OBJECT;
public:

    QVBoxLayout * m_MainLayout;

    ScriptFunctionInfo m_Function;
    ScriptManager * m_ScriptManager;

    inline ToolWidget(ScriptManager * sm, ScriptFunctionInfo f)
    {
        m_ScriptManager = sm;
        m_Function = f;

        m_MainLayout = new QVBoxLayout();
        m_MainLayout->setSizeConstraint( QLayout::SetFixedSize );
        this->setLayout(m_MainLayout);

        QLabel * functionname = new QLabel(QString(f.Function->GetName()));
        QFont font = functionname->font();
        font.setPointSize(15);
        functionname->setFont(font);

        QLabel * description = new QLabel();
        description->setWordWrap(false);
        description->setText(f.Description);


        QString text_in = "Input: ";
        QString text_out = "Output: ";

        for(int i = 0; i < f.Function->GetParamCount(); i++)
        {
            int typeId = 0;
            asDWORD flags = 0;
            const char * name;
            const char * def;
            f.Function->GetParam(i,&typeId,&flags,&name,&def);
            if(name != nullptr)
            {
                text_in +=" "+ QString((name));
            }
            asITypeInfo * ti = sm->m_Engine->GetTypeInfoById(typeId);
            if(ti != nullptr)
            {

                text_in +=  " ( " + QString(ti->GetName());
            }else
            {
                text_in += " ( ";
                text_in += sm->m_Engine->GetBaseTypeName(typeId);
            }
            if(def != nullptr)
            {
                text_in +=  " ,default = " + QString((def));
            }
            text_in += ") ";
        }

        int typeId = f.Function->GetReturnTypeId();

        asITypeInfo * ti =sm->m_Engine->GetTypeInfoById(typeId);
        if(ti != nullptr)
        {
            text_out += ti->GetName();
        }else
        {
            text_out += sm->m_Engine->GetBaseTypeName(typeId);
        }

        QLabel * functionin = new QLabel(text_in);
        QLabel * functionout = new QLabel(text_out);

        m_MainLayout->addWidget(functionname);
        m_MainLayout->addWidget(description);
        m_MainLayout->addWidget(functionin);
        m_MainLayout->addWidget(functionout);
        functionname->setMinimumSize(100,20);


    }
};


class ToolBrowser : public QWidget
{
        Q_OBJECT;

    ScriptManager * m_ScriptManager;
    DatabaseTool * m_ScriptTool;
    MapViewTool * m_MapViewTool;
    QStringListModel * m_ToolModel;




    QLineEdit * m_SearchEdit;
    QListWidget * m_ToolList;
    QVBoxLayout * m_MainLayout;

    QList<QListWidgetItem *> m_ListItems;
    QList<ToolWidget * > m_ListWidgets;
    QList<ScriptFunctionInfo> m_ListFunctions;
    QList<QString> m_Titles;
public:

    inline ToolBrowser(ScriptManager * sm, DatabaseTool * dbt, MapViewTool * mvt)
    {
        m_MapViewTool = mvt;
        m_ScriptManager = sm;
        m_ScriptTool = dbt;

        m_MainLayout = new QVBoxLayout();
        this->setLayout(m_MainLayout);

        m_SearchEdit = new QLineEdit();

        m_ToolList = new QListWidget();


        m_ToolModel = new QStringListModel();

        QList<ScriptFunctionInfo> items = sm->m_Engine->GetGlobalFunctionList();

        for(int i = 0; i < items.length(); i++)
        {
            QListWidgetItem * item = new QListWidgetItem();
            ToolWidget * w = new ToolWidget(sm,items.at(i));

            m_ListFunctions.append(items.at(i));
            m_ListWidgets.append(w);
            m_ListItems.append(item);
            m_Titles.append(QString(items.at(i).Function->GetDeclaration(true,false,true)));


            item->setSizeHint( w->sizeHint() );
            m_ToolList->addItem(item);
            m_ToolList->setItemWidget(item,w);
        }


        m_MainLayout->addWidget(m_SearchEdit);
        m_MainLayout->addWidget(m_ToolList);

        m_ToolList->setAlternatingRowColors(true);

        connect(m_SearchEdit, SIGNAL(textChanged(QString)),this,SLOT(OnSearchChanged(QString)));
        connect(m_ToolList,&QListWidget::itemDoubleClicked,this,&ToolBrowser::ItemDoubleClicked);

    }

public slots:


    inline void OnSearchChanged(QString t)
    {
        if(t.isEmpty())
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                m_ToolList->setItemHidden(m_ListItems.at(i),false);
            }
        }else
        {
            for(int i = 0; i < m_ListItems.length(); i++)
            {
                bool find = m_Titles.at(i).contains(t,Qt::CaseSensitivity::CaseInsensitive);
                m_ToolList->setItemHidden(m_ListItems.at(i),!find);
            }
        }
    }

    inline void ItemDoubleClicked(QListWidgetItem * i)
    {
        int index = m_ListItems.indexOf(i);
        if(index > -1)
        {
            ToolDialog * dl = new ToolDialog(m_ScriptManager,m_MapViewTool,m_ListFunctions.at(index));
            dl->exec();
        }

    }



};

