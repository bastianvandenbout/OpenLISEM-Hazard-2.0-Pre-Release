#ifndef LABELEDWIDGET_H
#define LABELEDWIDGET_H

#include "QWidget"
#include "QGroupBox"
#include "QHBoxLayout"
#include "QDialog"
#include "QToolButton"
#include "QCheckBox"
#include "colorwheel.h"
#include "QScrollArea"
#include "QToolBox"
#include "QSpacerItem"
#include "QLabel"

class QWidgetHDuo : public QWidget
{
    Q_OBJECT;
    public:

    QHBoxLayout * m_Layout;
    inline QWidgetHDuo(QWidget * w1, QWidget * w2) : QWidget()
    {

        m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);

        m_Layout->addWidget(w1);
        m_Layout->addWidget(w2);
    }

};

class QWidgetVDuo : public QWidget
{
    Q_OBJECT;
    public:

    QVBoxLayout * m_Layout;
    inline QWidgetVDuo(QWidget * w1, QWidget * w2) : QWidget()
    {

        m_Layout = new QVBoxLayout();
        this->setLayout(m_Layout);

        m_Layout->addWidget(w1);
        m_Layout->addWidget(w2);
    }

};

class QVLabeledWidget : public QWidget
{
    Q_OBJECT;
    public:

    QVBoxLayout * m_Layout;
    inline QVLabeledWidget(QString name, QWidget * w) : QWidget()
    {

        m_Layout = new QVBoxLayout();
        this->setLayout(m_Layout);

        m_Layout->addWidget(new QLabel(name));
        m_Layout->addWidget(w);
    }

};






class QLabeledWidget : public QWidget
{
    Q_OBJECT;
    public:

    QHBoxLayout * m_Layout;
    inline QLabeledWidget(QString name, QWidget * w) : QWidget()
    {

        m_Layout = new QHBoxLayout();
        this->setLayout(m_Layout);

        m_Layout->addWidget(new QLabel(name));
        m_Layout->addWidget(w);
    }

};


#endif // LABELEDWIDGET_H
