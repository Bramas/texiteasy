/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                          *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.       *                         *
 *                                                                         *
 ***************************************************************************/

#include "completionengine.h"
#include <QListWidgetItem>
#include <QHeaderView>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QtCore/qmath.h>
#include <QScrollBar>
#include "widgettextedit.h"
#include "widgettooltip.h"


CompletionEngine::CompletionEngine(WidgetTextEdit *parent) :
    QListWidget(parent),
    _widgetTextEdit(parent),
    _widgetTooltip(0),
    _commandBegin(QString(""))
{
    this->setVisible(false);


    this->loadFile(":/completion/tex.cwl");
    this->loadFile(":/completion/latex-document.cwl");
    this->loadFile(":/completion/latex-mathsymbols.cwl");
    this->loadFile(":/completion/amsmath.cwl");

    connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(cellSelected(int)));

    _words.removeDuplicates();
    _words.sort();
    qDebug()<<"Completion engine Initialized : "<<_words.count()<<" words";
}
CompletionEngine::~CompletionEngine()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete CompletionEngine";
#endif
}
void CompletionEngine::loadFile(QString filename)
{
    QFile userTagsfile(filename);

    if (!userTagsfile.open(QFile::ReadOnly)) return;

    QString line;
    QTextStream in(&userTagsfile);
    while (!in.atEnd())
    {
        line = in.readLine();
        if (!line.isEmpty()) _words.append(line.remove("\n"));
    }

}

void CompletionEngine::proposeCommand(int left, int top, int lineHeight, QString commandBegin)
{
    this->_commandBegin = commandBegin;
    if(commandBegin.length()<4)
    {
        this->parentWidget()->setFocus();
        return;
    }
    QRegExp commandRegex = QRegExp("^\\"+QString(commandBegin).replace('{',"\\{"), Qt::CaseInsensitive);
    this->addCustomWordFromSource(); //dont know where to put it but it seems to be a fast function so it's ok!
    QStringList found = this->_words.filter(commandRegex);//, Qt::CaseInsensitive));
    QStringList customFound = this->_customWords.filter(commandRegex);//, Qt::CaseInsensitive));
    found.append(customFound);
    found.sort();

    if(found.empty() || (
            found.count() == 1 && (found.first().length() == commandBegin.length() || found.first().indexOf('#')  == commandBegin.length())))
    {
        found.clear();
        this->parentWidget()->setFocus();
        return;
    }

    this->clear();
    this->setVisible(true);
    int idx = 0;
    int dieseIndex, tooltipIndex;
    foreach(const QString &word, found)
    {
        if((dieseIndex = word.indexOf(QRegExp("#"))) != -1)
        {
            this->insertItem(idx++,word.left(dieseIndex));
        }
        else
        if((tooltipIndex = word.indexOf('?')) != -1)
        {
            QListWidgetItem * item = new QListWidgetItem(word.left(tooltipIndex));
            item->setToolTip(word.right(word.size() - tooltipIndex - 1));
            this->insertItem(idx++, item);
        }
        else
        {
            this->insertItem(idx++,word);
        }

    }
    this->setItemSelected(this->item(0),true);
    this->setCurrentRow(0,QItemSelectionModel::Rows);

    QRect geo(0,0,250,qMin(150,this->count()*this->sizeHintForRow(0)+5));
    left = qMin(left,parentWidget()->width()-geo.width());
    geo.moveTo(QPoint(left, top));

    if(geo.bottom() > parentWidget()->height())
    {
        geo.translate(QPoint(0,-geo.height()-lineHeight-6));
    }
    this->setGeometry(geo);

}

void CompletionEngine::cellSelected(int row)
{
    if(_widgetTooltip)
    {
        delete _widgetTooltip;
        _widgetTooltip = 0;
    }
    if(row < 0)
    {
        return;
    }
    if(this->item(row)->toolTip().isEmpty())
    {
        return;
    }
    _widgetTooltip = new WidgetTooltip(this->parentWidget()->parentWidget()->parentWidget()->parentWidget());
    _widgetTooltip->setTopLeft(this->geometry().left() + this->width(),
                               this->geometry().top()+
                               16 *
                               (row-this->verticalScrollBar()->value()));
    _widgetTooltip->setText(this->item(row)->toolTip());
    _widgetTooltip->show();
}

QString CompletionEngine::acceptedWord()
{
    if(!this->selectedItems().count())
    {
        return QString("");
    }
    QString word = this->selectedItems().first()->text();

    QRegExp beginCommand("\\\\begin\\{([^\\}]+)\\}");
    if(word.indexOf(beginCommand) != -1)
    {
        QString environment = beginCommand.capturedTexts().last();
        QString endCommand(QString("\\end{")+environment+"}");

        if(word.indexOf("{\\n}") != -1) //the command takes care of the content between begin and end.
        {
            word.replace("{\\n}","\n");
            word += endCommand;
        }
        else
        {
            word += QString("\n    @text\n")+endCommand;
        }
    }
    return word;//.right(word.size() - _commandBegin.size());
}

void CompletionEngine::keyPressEvent(QKeyEvent *event)
{

    if(event->key() == Qt::Key_Alt ||
            event->key() == Qt::Key_AltGr ||
            event->key() == Qt::Key_Control ||
            event->key() == Qt::Key_Shift)
    {
        return;
    }
    if(event->key() == Qt::Key_Return ||
            event->key() == Qt::Key_Enter ||
            event->key() == Qt::Key_Up ||
            event->key() == Qt::Key_Down)
    {
        QListWidget::keyPressEvent(event);
        return;
    }
    if(event->text().contains(QRegExp("[^a-zA-Z\\{\\-_]")) || event->text().isEmpty())
    {
        this->setVisible(false);
        dynamic_cast<WidgetTextEdit*>(this->parentWidget())->setFocus(event);
        return;
    }
    dynamic_cast<WidgetTextEdit*>(this->parent())->insertPlainText(event->text());
}

void CompletionEngine::addCustomWordFromSource()
{
    _customWords.clear();
    QString source = dynamic_cast<WidgetTextEdit*>(this->parent())->toPlainText();
    QRegExp patternCommand("\\\\(re){0,1}newcommand\\{([^\\}]*)\\}");
    int index = source.indexOf(patternCommand);
    while(index != -1)
    {
        _customWords.append(patternCommand.capturedTexts().last());
        index = source.indexOf(patternCommand, index + 1);
    }


    QRegExp patternLabel("\\\\label\\{([^\\}]*)\\}");
    index = source.indexOf(patternLabel);
    while(index != -1)
    {
        _customWords.append("\\ref{"+patternLabel.capturedTexts().last()+"}");
        index = source.indexOf(patternLabel, index + 1);
    }

    QRegExp patternBibitem("\\\\bibitem\\{([^\\}]*)\\}");
    index = source.indexOf(patternBibitem);
    while(index != -1)
    {
        _customWords.append("\\cite{"+patternBibitem.capturedTexts().last()+"}");
        index = source.indexOf(patternBibitem, index + 1);
    }
    parseBibtexFile();
}

void CompletionEngine::parseBibtexFile()
{
    QString source = _widgetTextEdit->toPlainText();
    QRegExp patternBib("\\\\bibliography\\{([^\\}]*)\\}");
    int index = source.indexOf(patternBib);
    if(index == -1)
    {
        return;
    }

    QString filename = this->_widgetTextEdit->getCurrentFile()->getPath();

    filename += patternBib.capturedTexts().last();
    filename += ".bib";
    if(!QFile::exists(filename))
    {
        qDebug()<<"bibtex file does not exist : "<<filename;
        return;
    }

    QFile bibFile(filename);

    if(!bibFile.open(QFile::Text | QFile::ReadOnly))
    {
        qDebug()<<"failed to open bibtex file : "<<filename<<" "<<bibFile.errorString();
        return;
    }

    QList<BibItem> bibItemList = this->parseBibtexSource(bibFile.readAll());
    foreach(BibItem bibItem, bibItemList)
    {
        _customWords.append("\\cite{"+bibItem.key+"}?"+bibItem.title);
    }

}
QList<BibItem> CompletionEngine::parseBibtexSource(QString source)
{
    // Regex cannot work because of the possible nested brackets
    // So lets go with a little grammar parser that will keep the key and the title
    // of each bibtex item.
    enum{ OUTSIDE_ITEM, ITEM_TYPE, ITEM_KEY, FIELD_NAME, FIELD_VALUE, IGNORING_CHAR };

    QChar ignoredChar;
    QString::const_iterator currentChar = source.constBegin();
    int numberOfOpened = 0;
    int currentState = OUTSIDE_ITEM;

    QString tempString("");
    QString currentFieldName("");
    BibItem tempBibItem;
    QList<BibItem> bibItemList;

    while(currentChar != source.constEnd())
    {
        switch(currentState)
        {
        case OUTSIDE_ITEM:
            if((*currentChar) == '@')
            {
                currentState = ITEM_TYPE;
            }
        break;
        case ITEM_TYPE:
            if((*currentChar) == '{')
            {
                currentState = ITEM_KEY;
            }
        break;
        case ITEM_KEY:
            if((*currentChar) == ',')
            {
                currentState = FIELD_NAME;
                tempBibItem.key = tempString.trimmed();
                tempString.clear();
                break;
            }
            tempString.append(*currentChar);
        break;
        case FIELD_NAME:
            if((*currentChar) == '=')
            {
                currentFieldName = tempString.trimmed();
                tempString.clear();
                currentState = IGNORING_CHAR;
                ignoredChar = '{';
                break;
            }
            tempString.append(*currentChar);
        break;
        case FIELD_VALUE:

            if((*currentChar) == '{')
            {
                ++numberOfOpened;
            } else
            if((*currentChar) == '}' && --numberOfOpened == 0)
            {
                if(!currentFieldName.compare("title"))
                {
                    tempBibItem.title = tempString.trimmed();
                    bibItemList.append(tempBibItem);
                }
                tempString.clear();
                currentState = IGNORING_CHAR;
                ignoredChar = ',';
                break;
            }
            tempString.append(*currentChar);
        break;
        case IGNORING_CHAR:
            if((*currentChar) == ignoredChar)
            {
                if(ignoredChar == ',')
                {
                    currentState = FIELD_NAME;
                }
                else
                {
                    currentState = FIELD_VALUE;
                    numberOfOpened = 1;
                }
                break;
            }
            if((*currentChar) == '}' && ',' == ignoredChar)
            {
                currentState = OUTSIDE_ITEM;
            }
        break;
        }
        ++currentChar;
    }

    return bibItemList;
}
