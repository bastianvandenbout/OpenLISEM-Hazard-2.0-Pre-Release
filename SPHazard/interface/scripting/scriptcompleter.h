#ifndef SCRIPTCOMPLETER_H
#define SCRIPTCOMPLETER_H

#include <QCompleter>
#include "QStringListModel"
#include "QStandardItemModel"
#include "QTableView"
#include "QHeaderView"

#include "QToolTip"
#include "QLabel"
#include "QDialog"
#include "QVBoxLayout"
#include "QEvent"
#include "QMouseEvent"
#include "QDir"
#include "iomanager.h"

#include "iostream"

//table view for popup
class SPHScriptCompleterPopupView : public QTableView
{

    Q_OBJECT

    QDialog *popup;
     QLabel *popupLabel;
public:
     QStringList descriptions;

public:
    inline SPHScriptCompleterPopupView() : QTableView()
    {
        connect(selectionModel(),&QItemSelectionModel::selectionChanged,this,&SPHScriptCompleterPopupView::OnSelectionChanged);

        viewport()->installEventFilter(this);
        setMouseTracking(true);
        popup = new QDialog(this, Qt::Popup | Qt::ToolTip);

        QVBoxLayout *layout = new QVBoxLayout;
        popupLabel = new QLabel(popup);
        popupLabel->setWordWrap(true);
        layout->addWidget(popupLabel);
        popupLabel->setTextFormat(Qt::RichText);
        //popupLabel->setOpenExternalLinks(true);
        popup->setLayout(layout);
        popup->installEventFilter(this);
        hideColumn(1);

    }

    inline ~SPHScriptCompleterPopupView()
    {

        popup->hide();
    }

    bool eventFilter(QObject *watched, QEvent *event){
            if(viewport() == watched){
                if(event->type() == QEvent::MouseMove){
                    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                    QModelIndex index = indexAt(mouseEvent->pos());
                    if(index.isValid()){
                        showPopup(index);
                    }
                    else{
                        popup->hide();
                    }
                }
                else if(event->type() == QEvent::Leave){
                    popup->hide();
                }
            }
            else if(popup == watched){
                if(event->type() == QEvent::Leave){
                    popup->hide();
                }
            }
            return QTableView::eventFilter(watched, event);
        }

    void showPopup (const QModelIndex &index) const {
            if(index.column() == 0){
                int indexr = index.row();
                QRect r = visualRect(index);
                popup->move(viewport()->mapToGlobal(r.bottomRight()));
                popup->setFixedSize(300, popup->heightForWidth(300));

                popupLabel->setText(descriptions.at(indexr));
                popup->adjustSize();
                popup->show();
            }
            else {
                popup->hide();
            }
        }

    inline void UpdateToolTip()
    {
        /*QModelIndexList lrows = selectionModel()->selectedRows();


        std::cout << "update tooltip " << lrows.length() << std::endl;

        if(lrows.length() > 0)
        {
            QModelIndex index = model()->index(lrows.at(0).row(),1);
            if (!index.isValid())
            {
                QToolTip::hideText();
            }

           QString itemText = model()->data(index).toString();
            QString itemTooltip = model()->data(index, Qt::ToolTipRole).toString();

            QFontMetrics fm(font());
            int itemTextWidth = fm.width(itemText);
            QRect rect = visualRect(index);
            int rectWidth = rect.width();

            std::cout << "show text " << itemTooltip.toStdString() << std::endl;

           if ((itemTextWidth > rectWidth) && !itemTooltip.isEmpty())
            {


                QToolTip::showText(mapToGlobal(QPoint(0,0)), itemTooltip, this);
            }
            else
            {
                QToolTip::hideText();
            }

        }else {
            QToolTip::hideText();
        }*/

    }

public slots:

    inline void hideEvent(QHideEvent*e) override
    {
        popup->hide();
        QTableView::hideEvent(e);
    }

    inline void OnSelectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
    {

        UpdateToolTip();


    }
};



class SPHScriptCompleter : public QCompleter
{
    Q_OBJECT

public:
    inline SPHScriptCompleter(const QStringList& words,const QStringList& descriptions, QObject * parent) :
            QCompleter(parent), m_list(words), m_model()
    {
        m_descriptions = descriptions;
        m_word = words;

        //m_wordu = words;
        //m_descriptionsu = descriptions;

        for(int i = 0; i < m_word.length(); i++)
        {
            //if(!m_wordu.contains(m_word.at(i)))
            {
                m_wordu.append(m_word.at(i));
                m_descriptionsu.append(m_descriptions.at(i));
            }
        }

        connect(this, SIGNAL(highlighted(QModelIndex)),this,SLOT(OnHighLighted(QModelIndex)));

        m_CompleterModel = new QStandardItemModel(words.length(),1);
        for(int i = 0; i < m_wordu.length(); i++)
        {
            m_CompleterModel->setItem(i,0,new QStandardItem(m_wordu.at(i)));
            //m_CompleterModel->setItem(i,1,new QStandardItem("Description: " + m_wordu.at(i)));
        }

        setModel(m_CompleterModel);
        setCompletionColumn(0);

        tableView = new SPHScriptCompleterPopupView();

        tableView->verticalHeader()->setVisible(false);
        tableView->horizontalHeader()->setVisible(false);
        tableView->setSelectionBehavior(QTableView::SelectRows);
        tableView->setShowGrid(false);

        tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->hideColumn(1);

        //tableView->resizeColumnsToContents();
        //tableView->resizeRowsToContents();
        setPopup(tableView);

    }

    inline void update(QString word)
    {
        // Do any filtering you like.
        // Here we just include all items that contain word.
        QStringList filtered;
        QStringList filteredd;

        for(int i = 0; i < m_wordu.length(); i++)
        {
            if(m_wordu.at(i).startsWith(word, caseSensitivity()))
            {
                filtered.append(m_wordu.at(i));
                filteredd.append(m_descriptionsu.at(i));
            }

        }

        tableView->setVisible(false);

        QStringList filesf;
        QStringList filesfd;

        std::cout << "complete dir " << m_CurrentDir.toStdString() << std::endl;
        if(!m_CurrentDir.isEmpty())
        {
            QDir directory(m_CurrentDir);
            QStringList files = directory.entryList(IOManager::GetSupportedFileTypeExtensionsFilter(),QDir::Files);

            for(int i = 0; i < files.length(); i++)
            {
                if(files.at(i).startsWith(word, caseSensitivity()))
                {
                    filesf.append(files.at(i));
                    filesfd.append(m_CurrentDir + QDir::separator() + files.at(i));

                }
            }
        }


        m_CompleterModel->clear();
        m_CompleterModel->setColumnCount(1);
        m_CompleterModel->setRowCount(filtered.length() + filesf.length());


        for(int i = 0; i < filtered.length(); i++)
        {
            m_CompleterModel->setItem(i,0,new QStandardItem(filtered.at(i)));

            //m_CompleterModel->setItem(i,1,new QStandardItem(filteredd.at(i)));
        }

        for(int i = 0; i < filesf.length(); i++)
        {
            m_CompleterModel->setItem(filtered.length() + i,0,new QStandardItem(filesf.at(i)));

            filteredd.append(filesfd.at(i));
        }


        tableView->descriptions = filteredd;

        m_model.setStringList(filtered);
        m_currentword = word;

        tableView->setVisible(true);

        complete();
        this->setCompletionPrefix(word);

    }

    inline void ShowPopup0()
    {

        tableView->showPopup(tableView->model()->index(0,0));
    }

    inline QString word()
    {
        return m_currentword;
    }

    inline void SetDirectory(QString dir)
    {
        m_CurrentDir = dir;
    }
public slots:
    inline void OnHighLighted(QModelIndex index)
    {
        tableView->showPopup(index);
    }

private:
    QStringList m_list;
    QStringListModel m_model;
    QStandardItemModel *m_CompleterModel;

    SPHScriptCompleterPopupView * tableView;
    QString m_currentword;
    QStringList m_word;
    QStringList m_descriptions;
    QStringList m_wordu;
    QStringList m_descriptionsu;

    QString m_CurrentDir;
};


#endif // SCRIPTCOMPLETER_H
