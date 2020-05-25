#include <QtWidgets>

#include <QCompleter>
#include "codeeditor.h"
#include "sphsyntaxhighlighter.h"

CodeEditor::CodeEditor(QWidget *parent, ScriptManager * sm) : QPlainTextEdit(parent)
{
    m_ScriptManager = sm;
    lineNumberArea = new LineNumberArea(this);

    m_FindReplaceDialog = new FindReplaceDialog(this);
    m_FindReplaceDialog->setModal(false);
    m_FindReplaceDialog->setTextEdit(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateExtraSelection()));
    connect(this, SIGNAL(textChanged()),this,SLOT(OnTextChanged()));


    //connect(this,&QTextEdit::selectionChanged,this,&CodeEditor::onSelectionChanged);

    m_File = "";
    m_FileName = "<new>";
    m_HasBeenEditedSinceSave = false;


    updateLineNumberAreaWidth(0);

    QString m_FontDir;
    m_FontDir = QCoreApplication::applicationDirPath()+"/fonts/";

    //try to find custom font
    int id = QFontDatabase::addApplicationFont(m_FontDir + "dejavu/DejaVuSansMono.ttf");
    if(id != -1)
    {
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont FiraCode(family);
        FiraCode.setWeight(QFont::Normal);
        FiraCode.setPointSize(10);
        FiraCode.setStyleHint(QFont::Monospace);
        this->setFont(FiraCode);

        const int tabStop = 4;  // 4 characters

        QFontMetrics metrics(FiraCode);
        setTabStopWidth(tabStop * metrics.width(' '));

    }else
    {
        SPH_DEBUG("Could not find font " + m_FontDir + "firacode/FiraCode-Regular.ttf");
    }

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    Highlighter * SPHHighlighter = new Highlighter(this->document());

    //QPalette p = palette(); // define pallete for textEdit..
    //p.setColor(QPalette::Base, QColor(250,250,250)); // set color "Red" for textedit base
    //setPalette(p); // change textedit palette

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


    emit OnEditedSinceSave();
}

void CodeEditor::updateExtraSelection()
{
    QList<QTextEdit::ExtraSelection> extra;

    highlightCurrentLine(extra);
    highlightParenthesis(extra);

    setExtraSelections(extra);
}

void CodeEditor::onSelectionChanged()
{
    auto selected = textCursor().selectedText();

    auto cursor = textCursor();

    // Cursor is null if setPlainText was called.
    if (cursor.isNull())
    {
        return;
    }

    /*cursor.movePosition(QTextCursor::MoveOperation::Left);
    cursor.select(QTextCursor::SelectionType::WordUnderCursor);

    QSignalBlocker blocker(this);
    m_framedAttribute->clear(cursor);

    if (selected.size() > 1 &&
        cursor.selectedText() == selected)
    {
        auto backup = textCursor();

        // Perform search selecting
        handleSelectionQuery(cursor);

        setTextCursor(backup);
    }*/
}

int CodeEditor::getIndentationSpaces()
{
    auto blockText = textCursor().block().text();

    int indentationLevel = 0;

    for (auto i = 0;
         i < blockText.size() && QString("\t ").contains(blockText[i]);
         ++i)
    {
        if (blockText[i] == ' ')
        {
            indentationLevel++;
        }
        else
        {
#if QT_VERSION >= 0x050A00
            indentationLevel += tabStopDistance() / fontMetrics().averageCharWidth();
#else
            indentationLevel += tabStopWidth() / fontMetrics().averageCharWidth();
#endif
        }
    }

    return indentationLevel;
}

QChar CodeEditor::CharUnderCursor(int offset) const
{
    auto block = textCursor().blockNumber();
    auto index = textCursor().positionInBlock();
    auto text = document()->findBlockByNumber(block).text();

    index += offset;

    if (index < 0 || index >= text.size())
    {
        return {};
    }

    return text[index];
}


QString CodeEditor::wordUnderCursor() const
{
    auto tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    bool m_replaceTab = true;
    bool m_autoIndentation = true;
    bool m_autoParentheses = true;
    QString m_tabReplace;
    m_tabReplace.clear();
    m_tabReplace.fill(' ', 4);

    #if QT_VERSION >= 0x050A00
      const int defaultIndent = tabStopDistance() / fontMetrics().averageCharWidth();
    #else
      const int defaultIndent = tabStopWidth() / fontMetrics().averageCharWidth();
    #endif



    if (m_Completer && m_Completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    bool isShortcutSearch = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_F); // CTRL+E


    if(isShortcutSearch)
    {
        m_FindReplaceDialog->show();
    }

    if(!m_Completer || !isShortcut)
    {
        // do not process the shortcut when we have a completer


        if (m_replaceTab && e->key() == Qt::Key_Tab &&
            e->modifiers() == Qt::NoModifier) {
          insertPlainText(m_tabReplace);
          return;
        }

        int indentationLevel = getIndentationSpaces();
        #if QT_VERSION >= 0x050A00
            int tabCounts =
                indentationLevel * fontMetrics().averageCharWidth() / tabStopDistance();
        #else
            int tabCounts =
                indentationLevel * fontMetrics().averageCharWidth() / tabStopWidth();
        #endif

        if (m_autoIndentation &&
               (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) &&
                CharUnderCursor() == '}' && CharUnderCursor(-1) == '{')
            {
              int charsBack = 0;
              insertPlainText("\n");

              if (m_replaceTab)
                insertPlainText(QString(indentationLevel + defaultIndent, ' '));
              else
                insertPlainText(QString(tabCounts + 1, '\t'));

              insertPlainText("\n");
              charsBack++;

              if (m_replaceTab)
              {
                insertPlainText(QString(indentationLevel, ' '));
                charsBack += indentationLevel;
              }
              else
              {
                insertPlainText(QString(tabCounts, '\t'));
                charsBack += tabCounts;
              }

              while (charsBack--)
                moveCursor(QTextCursor::MoveOperation::Left);
              return;
            }

            // Shortcut for moving line to left
            if (m_replaceTab && e->key() == Qt::Key_Backtab) {
              indentationLevel = std::min(indentationLevel, m_tabReplace.size());

              auto cursor = textCursor();

              cursor.movePosition(QTextCursor::MoveOperation::StartOfLine);
              cursor.movePosition(QTextCursor::MoveOperation::Right,
                                  QTextCursor::MoveMode::KeepAnchor, indentationLevel);

              cursor.removeSelectedText();
              return;
            }


             QPlainTextEdit::keyPressEvent(e);

            if (m_autoIndentation && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)) {
                  if (m_replaceTab)
                    insertPlainText(QString(indentationLevel, ' '));
                  else
                    insertPlainText(QString(tabCounts, '\t'));
                }


            if (m_autoParentheses)
            {
              for (auto&& el : parentheses)
              {
                        // Inserting closed brace
                        if (el.first == e->text())
                        {
                          insertPlainText(el.second);
                          moveCursor(QTextCursor::MoveOperation::Left);
                          break;
                        }

                        // If it's close brace - check parentheses
                        if (el.second == e->text())
                        {
                            auto symbol = CharUnderCursor();

                            if (symbol == el.second)
                            {
                                textCursor().deletePreviousChar();
                                moveCursor(QTextCursor::MoveOperation::Right);
                            }

                            break;
                        }
                    }
              }



    }



    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!m_Completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                      || eow.contains(e->text().right(1)))) {

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

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (m_Completer)
        m_Completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}
void CodeEditor::setCompleter(SPHScriptCompleter *completer)
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
                     this, &CodeEditor::insertCompletion);
}

QCompleter *CodeEditor::completer() const
{
    return m_Completer;
}
void CodeEditor::insertCompletion(const QString& completion)
{
    if (m_Completer->widget() != this)
    {
        return;
    }

    std::cout << "completion " << completion.toStdString() << " " <<  m_Completer->completionPrefix().length() << std::endl;

    QTextCursor tc = textCursor();
    int extra = completion.length() - m_Completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 +  fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine(QList<QTextEdit::ExtraSelection>& extraSelection)
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);

    }

}

void CodeEditor::highlightParenthesis(QList<QTextEdit::ExtraSelection>& extraSelection)
{
    auto currentSymbol = CharUnderCursor();
    auto prevSymbol = CharUnderCursor(-1);

    for (auto& pair : parentheses)
    {
        int direction;

        QChar counterSymbol;
        QChar activeSymbol;
        auto position = textCursor().position();

        if (pair.first == currentSymbol)
        {
            direction = 1;
            counterSymbol = pair.second[0];
            activeSymbol = currentSymbol;
        }
        else if (pair.second == prevSymbol)
        {
            direction = -1;
            counterSymbol = pair.first[0];
            activeSymbol = prevSymbol;
            position--;
        }
        else
        {
            continue;
        }

        auto counter = 1;

        while (counter != 0 &&
               position > 0 &&
               position < (document()->characterCount() - 1))
        {
            // Moving position
            position += direction;

            auto character = document()->characterAt(position);
            // Checking symbol under position
            if (character == activeSymbol)
            {
                ++counter;
            }
            else if (character == counterSymbol)
            {
                --counter;
            }
        }

        QTextCharFormat format = QTextCharFormat();
        format.setBackground(Qt::yellow);

        // Found
        if (counter == 0)
        {
            QTextEdit::ExtraSelection selection{};

            auto directionEnum =
                 direction < 0 ?
                 QTextCursor::MoveOperation::Left
                 :
                 QTextCursor::MoveOperation::Right;

            selection.format = format;
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            selection.cursor.movePosition(
                directionEnum,
                QTextCursor::MoveMode::MoveAnchor,
                std::abs(textCursor().position() - position)
            );

            selection.cursor.movePosition(
                QTextCursor::MoveOperation::Right,
                QTextCursor::MoveMode::KeepAnchor,
                1
            );

            extraSelection.append(selection);

            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            selection.cursor.movePosition(
                directionEnum,
                QTextCursor::MoveMode::KeepAnchor,
                1
            );

            extraSelection.append(selection);
        }

        break;
    }
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            m_CurrentRunLineMutex.lock();
            if((blockNumber + 1) == m_CurrentRunLine)
            {
                 painter.setBrush(Qt::yellow);
                 painter.drawEllipse(fontMetrics().height()/2, top ,fontMetrics().height(),fontMetrics().height());
            }
            m_CurrentRunLineMutex.unlock();
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);

        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
