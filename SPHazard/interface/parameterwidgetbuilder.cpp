#include "qtinterfacewindow.h"
#include "iomanager.h"

void QTInterfaceWindow::CreateParameterWidgets()
{


    QTabWidget * m_TabModel = new QTabWidget();

    TabWidget->addTab(m_TabModel,"SPH Model");

    QList<QVBoxLayout *> pastparents;
    QVBoxLayout * currentparent = m_Layout;
    pastparents.append(m_Layout);
    int currentlevel = -1;

    TabWidgetLayouts.clear();

    SPH_DEBUG("Using parameter list to automatically generate widgets")

    for(int i = 0; i < m_ParameterManager->m_Parameters.length(); i++)
    {

        SPHParameter p = m_ParameterManager->m_Parameters.at(i);

        if(p.m_level > -1)
        {


            ParameterWidget pw;
            pw.m_SpinBox = 0;
            pw.m_CheckBox = 0;
            pw.m_DSpinBox = 0;
            pw.m_LineEdit = 0;
            pw.m_RadioButtons.clear();
            pw.m_ParameterIndex = i;
            pw.m_ParameterName = p.m_Name;


            int pwindex = m_ParameterWidgetList.length();


            bool is_grouplabel = false;
            if(p.m_level > currentlevel)
            {
                is_grouplabel = true;
                if(p.m_Type == SPH_PARAMETER_LABEL)
                {
                    if(p.m_level == 0)
                    {
                        QWidget *tab = new QWidget();

                        QScrollArea *tab_scrollArea = new QScrollArea;
                        tab_scrollArea->setWidgetResizable(true);
                        tab_scrollArea->setWidget(tab);
                        tab_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
                        tab_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                        QVBoxLayout *gblayout = new QVBoxLayout;
                        gblayout->addWidget(tab_scrollArea);

                        QVBoxLayout *gblayout2 = new QVBoxLayout;
                        tab->setLayout(gblayout2);
                        gblayout2->addWidget(tab);

                        m_TabModel->addTab(tab_scrollArea,p.m_Name);

                        TabWidgetLayouts.append(gblayout2);

                        pastparents.append(currentparent);
                        currentparent = gblayout2;
                    }else
                    {
                        QGroupBox * gb = new QGroupBox(p.m_Name);
                        gb->setFont(QFont( "Helvetica", 15 ));
                        QVBoxLayout *gblayout = new QVBoxLayout;
                        gb->setLayout(gblayout);
                        gblayout->setMargin(0);
                        gblayout->setSpacing(0);
                        currentparent->addWidget(gb);
                        pastparents.append(currentparent);
                        currentparent = gblayout;
                    }

                }

            }else if(p.m_level < currentlevel)
            {
                is_grouplabel = true;
                for(int j = currentlevel; j > std::max(0,p.m_level); j--)
                {
                    if(pastparents.length() > 0)
                    {
                        currentparent = pastparents.at(pastparents.length()-1);
                        pastparents.removeLast();
                    }
                }

                QGroupBox * gb = new QGroupBox(p.m_Name);
                gb->setFont(QFont( "Helvetica", 15 ));
                gb->setToolTip(p.m_Description);
                QVBoxLayout *gblayout = new QVBoxLayout;
                gb->setLayout(gblayout);
                currentparent->addWidget(gb);
                pastparents.append(currentparent);
                currentparent = gblayout;
            }

            currentlevel = p.m_level;

            if(p.m_Type == SPH_PARAMETER_LABEL && !is_grouplabel)
            {
                QLabel *l = new QLabel();
                l->setText(p.m_Name);
                l->setFont(QFont( "Helvetica", 10 ));
                currentparent->addWidget(l);

            }else if(p.m_Type == SPH_PARAMETER_DIR)
            {
                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                QLineEdit *l = new QLineEdit();
                l->setText(p.m_Value);
                l->setFont(QFont( "Helvetica", 10 ));
                templayout->addWidget(l);

                QIcon icon;
                icon.addFile((m_Dir + SPH_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);


                connect(addButton,SIGNAL(pressed()),m_SignalMapper_Dir,SLOT(map()));
                m_SignalMapper_Dir->setMapping(addButton,pwindex);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);
                templayout->addWidget(addButton);
                templayout->setSpacing(0);
                templayout->setMargin(0);
                currentparent->addWidget(hw);

                pw.m_LineEdit = l;

            }else if(p.m_Type == SPH_PARAMETER_FILE)
            {
                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                QLabel * lab = new QLabel(p.m_Name);
                lab->setFont(QFont( "Helvetica", 10 ));
                lab->setMinimumSize(120,22);
                lab->setMaximumSize(120,22);
                lab->setToolTip(p.m_Description);
                templayout->addWidget(lab);


                QLineEdit *l = new QLineEdit();
                l->setText(p.m_Value);
                l->setFont(QFont( "Helvetica", 10 ));
                templayout->addWidget(l);

                if(p.m_hasmult)
                {
                    QDoubleSpinBox * dsb = new QDoubleSpinBox();
                    dsb->setValue(p.m_mult);
                    dsb->setFont(QFont( "Helvetica", 10 ));
                    templayout->addWidget(dsb);
                    pw.m_DSpinBox = dsb;
                }


                QIcon icon;
                icon.addFile((m_Dir + SPH_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);


                connect(addButton,SIGNAL(pressed()),m_SignalMapper_File,SLOT(map()));
                m_SignalMapper_File->setMapping(addButton,pwindex);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);
                templayout->addWidget(addButton);
                templayout->setSpacing(0);
                templayout->setMargin(0);
                currentparent->addWidget(hw);

                pw.m_LineEdit = l;

            }else if(p.m_Type == SPH_PARAMETER_BOOL)
            {

                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                QCheckBox *l = new QCheckBox(p.m_Name);
                l->setFont(QFont( "Helvetica", 10 ));
                l->setToolTip(p.m_Description);
                templayout->addWidget(l);

                currentparent->addWidget(hw);

                pw.m_CheckBox = l;

            }
            if(p.m_Type == SPH_PARAMETER_MAP)
            {
                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(1);
                templayout->setMargin(0);


                QLabel * lab = new QLabel(p.m_Name);
                lab->setFont(QFont( "Helvetica", 10 ));
                lab->setMinimumSize(120,22);
                lab->setMaximumSize(120,22);
                lab->setToolTip(p.m_Description);
                templayout->addWidget(lab);

                QLineEdit *l = new QLineEdit();
                l->setText(p.m_Value);
                l->setFont(QFont( "Helvetica", 10 ));
                templayout->addWidget(l);


                if(p.m_hasmult)
                {
                    QDoubleSpinBox * dsb = new QDoubleSpinBox();
                    dsb->setValue(p.m_mult);
                    dsb->setFont(QFont( "Helvetica", 10 ));
                    templayout->addWidget(dsb);
                    pw.m_DSpinBox = dsb;
                }

                QIcon icon;
                icon.addFile((m_Dir + SPH_FOLDER_ASSETS + "fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);

                QToolButton *addButton = new QToolButton(this);
                addButton->setIcon(icon);
                addButton->setIconSize(QSize(22,22));
                addButton->resize(22,22);
                addButton->setEnabled(true);

                connect(addButton,SIGNAL(pressed()),m_SignalMapper_Map,SLOT(map()));
                m_SignalMapper_Map->setMapping(addButton,pwindex);

                QString buttonStyle = "QPushButton{border:none;background-color:rgba(255, 255, 255,100);}";
                addButton->setStyleSheet(buttonStyle);
                addButton->setMinimumSize(22,22);
                addButton->setMaximumSize(22,22);
                templayout->addWidget(addButton);

                currentparent->addWidget(hw);

                pw.m_LineEdit = l;

            }else if(p.m_Type == SPH_PARAMETER_FLOAT)
            {


                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                if(p.m_minrange == p.m_maxrange)
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QLineEdit *l = new QLineEdit();
                    l->setText(p.m_Value);
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_LineEdit = l;
                }else
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QDoubleSpinBox *l = new QDoubleSpinBox();
                    l->setValue(p.m_Value.toFloat());
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_DSpinBox = l;
                }
                currentparent->addWidget(hw);

            }else if(p.m_Type == SPH_PARAMETER_INT)
            {

                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                if(p.m_minrange == p.m_maxrange)
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QLineEdit *l = new QLineEdit();
                    l->setText(p.m_Value);
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_LineEdit = l;
                }else
                {
                    QLabel *la = new QLabel(p.m_Name);
                    la->setFont(QFont( "Helvetica", 10 ));
                    QSpinBox *l = new QSpinBox();
                    l->setValue(p.m_Value.toFloat());
                    l->setFont(QFont( "Helvetica", 10 ));
                    l->setToolTip(p.m_Description);
                    templayout->addWidget(la);
                    templayout->addWidget(l);
                    pw.m_SpinBox = l;
                }
                currentparent->addWidget(hw);
            }else if(p.m_Type == SPH_PARAMETER_MULTICHECK)
            {

                QWidget * hw = new QWidget();
                QHBoxLayout *templayout = new QHBoxLayout(hw);
                hw->setLayout(templayout);
                templayout->setSpacing(0);
                templayout->setMargin(0);

                currentparent->addWidget(hw);
            }

            m_ParameterWidgetList.append(pw);


        }else
        {
            if(p.m_level < currentlevel)
            {
                for(int j = currentlevel; j > std::max(0,p.m_level); j--)
                {
                    if(pastparents.length() > 0)
                    {
                        currentparent = pastparents.at(pastparents.length()-1);
                        pastparents.removeLast();
                    }
                }
            }
            currentlevel = -1;

        }
    }

    for(int i = 0; i < TabWidgetLayouts.length(); i++)
    {
        QVBoxLayout * l = TabWidgetLayouts.at(i);
        l->addStretch();
    }
}

void QTInterfaceWindow::UpdateParameters()
{
    for(int i =0; i< m_ParameterWidgetList.length(); i++)
    {
        ParameterWidget pw = m_ParameterWidgetList.at(i);
        bool found = false;
        if(pw.m_SpinBox != 0)
        {
            found = true;
            m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_SpinBox->value());
        }else if(pw.m_LineEdit != 0)
        {
            found = true;
            m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_LineEdit->text());
        }else if(pw.m_CheckBox != 0)
        {
            found = true;
            m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_CheckBox->isChecked());
        }else if(pw.m_RadioButtons.length() > 0)
        {
            found = true;
        }

        if(pw.m_DSpinBox != 0)
        {
            if(found)
            {
                m_ParameterManager->SetParameterMultValue(pw.m_ParameterIndex,pw.m_DSpinBox->value());
            }else
            {
                m_ParameterManager->SetParameterValue(pw.m_ParameterIndex,pw.m_DSpinBox->value());
            }
        }
    }
}

void QTInterfaceWindow::UpdateInterface()
{
    for(int i =0; i< m_ParameterWidgetList.length(); i++)
    {
        ParameterWidget pw = m_ParameterWidgetList.at(i);
        {
            bool found = false;
            if(pw.m_SpinBox != 0)
            {
                found = true;
                pw.m_SpinBox->setValue(m_ParameterManager->GetParameterValueInt(pw.m_ParameterIndex));
            }else if(pw.m_LineEdit != 0)
            {
                found = true;
                pw.m_LineEdit->setText(m_ParameterManager->GetParameterValueString(pw.m_ParameterIndex));

            }else if(pw.m_CheckBox != 0)
            {
                found = true;
                pw.m_CheckBox->setChecked(m_ParameterManager->GetParameterValueBool(pw.m_ParameterIndex));
            }else if(pw.m_RadioButtons.length() > 0)
            {
                found = true;
            }

            if(pw.m_DSpinBox != 0)
            {
                if(found)
                {
                    pw.m_DSpinBox->setValue(m_ParameterManager->GetParameterMultValueDouble(pw.m_ParameterIndex));
                }else
                {
                    pw.m_DSpinBox->setValue(m_ParameterManager->GetParameterValueDouble(pw.m_ParameterIndex));
                }
            }
        }
    }
}

void QTInterfaceWindow::SignalFunction_Bool(int index)
{


}

void QTInterfaceWindow::SignalFunction_Map(int index)
{

    ParameterWidget pw = m_ParameterWidgetList.at(index);
    SPHParameter p = m_ParameterManager->m_Parameters.at(pw.m_ParameterIndex);

    QString dir = m_ParameterManager->GetParameterValueString("Map Directory");

    if(dir.isEmpty())
    {
        QString currentDir = QCoreApplication::applicationDirPath()+"/";

        if (m_DropDown->count() > 0)
        {
            dir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
        }
        else
        {
            dir = currentDir;
        }
    }
    QString path = QFileDialog::getOpenFileName(this,	QString("Select the map: %1;").arg(pw.m_ParameterName),
                                                dir,QString("*.map *.tif *.csf;;*.*"));
    // open file dialog


    if (!path.isEmpty())// && QFileInfo(path).exists())
    {
       if(!rasterCanBeOpenedForReading(path)) {
          QMessageBox::critical(this, "SPHazard",
             QString("File \"%1\" is not a supported raster map.")
                 .arg(path));
          return;
       }

       QString f = QFileInfo(path).fileName();
       QString p = QFileInfo(path).dir().path();

       pw.m_LineEdit->setText(f);

    }else
    {
        /*if(!rasterCanBeOpenedForReading(path)) {
           QMessageBox::critical(this, "SPHazard",
              QString("Empty file name: %1.")
                  .arg(path));
           return;
        }*/

    }
}

void QTInterfaceWindow::SignalFunction_Dir(int index)
{
    ParameterWidget pw = m_ParameterWidgetList.at(index);
    SPHParameter p = m_ParameterManager->m_Parameters.at(pw.m_ParameterIndex);

    QString dir;
    QString currentDir = QCoreApplication::applicationDirPath()+"/";

    if (m_DropDown->count() > 0)
    {
        dir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
    }
    else
    {
        dir = currentDir;
    }

    QString path = QFileDialog::getExistingDirectory(this, QString("Select a directory"),
                                                             dir,
                                                             QFileDialog::ShowDirsOnly
                                                             | QFileDialog::DontResolveSymlinks);

    // open file dialog

    if (!path.isEmpty())// && QFileInfo(path).exists())
    {

        pw.m_LineEdit->setText(QDir::cleanPath(path) + "/");
    }else
    {
        /*if(!rasterCanBeOpenedForReading(path)) {
           QMessageBox::critical(this, "SPHazard",
              QString("Empty file name: %1.")
                  .arg(path));
           return;
        }*/

    }

}

void QTInterfaceWindow::SignalFunction_File(int index)
{
    ParameterWidget pw = m_ParameterWidgetList.at(index);
    SPHParameter p = m_ParameterManager->m_Parameters.at(pw.m_ParameterIndex);



    QString dir;
    QString currentDir = QCoreApplication::applicationDirPath()+"/";

    if (m_DropDown->count() > 0)
    {
        dir = QFileInfo(m_DropDown->currentText()).dir().absolutePath();
    }
    else
    {
        dir = currentDir;
    }

    QString path = QFileDialog::getOpenFileName(this,	QString("Select the map: %1;").arg(pw.m_ParameterName),
                                                dir,QString("*.*"));
    // open file dialog


    if (!path.isEmpty())// && QFileInfo(path).exists())
    {
       QString f = QFileInfo(path).fileName();
       QString p = QFileInfo(path).dir().path();

       pw.m_LineEdit->setText(f);

    }else
    {
        /*if(!rasterCanBeOpenedForReading(path)) {
           QMessageBox::critical(this, "SPHazard",
              QString("Empty file name: %1.")
                  .arg(path));
           return;
        }*/

    }

}

void QTInterfaceWindow::SignalFunction_Float(int index)
{


}

void QTInterfaceWindow::SignalFunction_Int(int index)
{


}

void QTInterfaceWindow::SignalFunction_MultiCheck(int index)
{


}
