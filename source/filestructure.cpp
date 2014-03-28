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

#include "filestructure.h"
#include "widgettextedit.h"
#include "blockdata.h"
#include <QList>
#include <QTextBlock>
#include <QDebug>


TextStruct::TextStruct(WidgetTextEdit * parent) :
    _widgetTextEdit(parent)
{

}

void TextStruct::clear(StructItem * item)
{
    foreach(StructItem * child, item->children)
    {
        clear(child);
        delete child;
    }
    item->children.clear();
}

void TextStruct::clear()
{
    clear(&_environementRoot);
    clear(&_sectionRoot);
}

void TextStruct::reload()
{
    clear();
    QTextBlock block = _widgetTextEdit->document()->begin();
    //QStack<StructItem*> structItemsStack;

    StructItem * currentEnvironmentItem = &_environementRoot;
    StructItem * currentSectionItem = &_sectionRoot;
    while(block.isValid())
    {
        BlockData * data = dynamic_cast<BlockData*>(block.userData());
        if(!data)
        {
            block = block.next();
            continue;
        }
        foreach(LatexBlockInfo* blockInfo, data->latexblocks())
        {
            switch(blockInfo->type)
            {
            case LatexBlockInfo::ENVIRONEMENT_BEGIN:
            {
                //qDebug()<<"ENVIRONEMENT_BEGIN : "<<blockInfo->name;
                StructItem * item = new StructItem();
                item->type   = StructItem::ENVIRONMENT;
                item->parent = currentEnvironmentItem;
                item->name   = blockInfo->name;
                item->level  = currentEnvironmentItem->level + 1;
                item->begin  = item->end  = blockInfo->position + block.position();
                item->blockBeginNumber  = item->blockEndNumber  = blockInfo->blockNumber;
                currentEnvironmentItem->children.append(item);
                currentEnvironmentItem = item;
            }
                break;
            case LatexBlockInfo::ENVIRONEMENT_END:
                //qDebug()<<"ENVIRONEMENT_END : "<<blockInfo->name<<"  current:"<<currentEnvironmentItem->name;
                if(currentEnvironmentItem->name.compare(blockInfo->name))
                {
                    qDebug()<<"Error Parsing Document Structur";
                    return;
                }
                currentEnvironmentItem->end   = blockInfo->position + block.position();
                currentEnvironmentItem->blockEndNumber  = blockInfo->blockNumber;
                currentEnvironmentItem = currentEnvironmentItem->parent;
                qDebug()<<" change current: "<<currentEnvironmentItem->name;
                break;
            case LatexBlockInfo::SECTION:
                //close previous sections
                while(blockInfo->sectionLevel <= currentSectionItem->level && currentSectionItem->parent)
                {
                    currentSectionItem->end   = blockInfo->position + block.position();
                    currentSectionItem->blockEndNumber  = blockInfo->blockNumber;
                    currentSectionItem = currentSectionItem->parent;
                }
                //creacte new section
                {
                    //qDebug()<<"SECTION : "<<blockInfo->name;
                    StructItem * item = new StructItem();
                    item->type   = StructItem::SECTION;
                    item->parent = currentSectionItem;
                    item->name   = blockInfo->name;
                    item->level  = blockInfo->sectionLevel;
                    item->begin  = item->end  = blockInfo->position + block.position();
                    item->blockBeginNumber  = item->blockEndNumber  = blockInfo->blockNumber;
                    currentSectionItem->children.append(item);
                    currentSectionItem = item;
                }


            }
        }
        block = block.next();

    }
}

QStack<const StructItem*> TextStruct::environmentPath() const
{
    return environmentPath(_widgetTextEdit->textCursor().position());
}

QStack<const StructItem*> TextStruct::environmentPath(int position) const
{
    QStack<const StructItem*> path;
    const StructItem * currentItem = &_environementRoot;
    path.append(currentItem);

    while(currentItem->children.count())
    {
        bool isInAChild = false;
        foreach(const StructItem * child, currentItem->children)
        {
            if(child->begin < position && child->end > position)
            {
                path << child;
                currentItem = child;
                isInAChild = true;
                break;
            }
        }
        if(!isInAChild)
        {
            break;
        }
    }
    return path;
}

void TextStruct::debug()
{
    qDebug()<<"Environement:";
    debug(&_environementRoot, 0);
    qDebug()<<"Section:";
    debug(&_sectionRoot, 0);
}

void TextStruct::debug(StructItem * item, int level)
{
    qDebug()<<QString().fill('|',level)+" "+QString::number(item->begin)+" "+item->name;
    foreach(StructItem * child, item->children)
    {
        debug(child, level + 1);
    }
    qDebug()<<QString().fill('|',level)+"- "+QString::number(item->end)+" /"+item->name;
}




FileStructure::FileStructure(WidgetTextEdit *parent) :
    blockIndentations(new BlockIndentation[0]),
    structureInfo(new QList<FileStructureInfo*>()),
    widgetTextEdit(parent)
{

}
FileStructure::~FileStructure()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete FileStructure";
#endif
    delete this->blockIndentations;
    structureInfo->clear();
    delete structureInfo;
}

void FileStructure::updateStructure()
{
    // clean memory
    foreach(FileStructureInfo * in,*this->structureInfo)
    {
        delete in;
    }
    this->structureInfo->clear();
    delete this->blockIndentations;
    this->blockIndentations = new BlockIndentation[this->widgetTextEdit->document()->blockCount()];

    int textBlockIndex = 0;
    FileStructureInfo * stru;
    int lastLevel[3];
    lastLevel[0] = lastLevel[1] = lastLevel[2] = -1;
    int level = 0;
    int lastBlock=0;
    QTextBlock textBlock = this->widgetTextEdit->document()->begin();
    bool first = true;
    do{

        if(textBlock.text().indexOf(QRegExp("\\\\(sub){0,2}section\\{[^\\}]*\\}"), 0) != -1 ||
                textBlock.text().indexOf(QRegExp("\\\\begin\\{thebibliography\\}"), 0) != -1)
        {
            for(int i = lastBlock; i < textBlockIndex; ++i)
            {
                this->blockIndentations[i].level = level;
                this->blockIndentations[i].next = textBlockIndex;
            }
            level = textBlock.text().indexOf(QRegExp("\\\\subsubsection\\{[^\\}]*\\}"), 0) != -1 ? 3 : textBlock.text().indexOf(QRegExp("\\\\subsection\\{[^\\}]*\\}"), 0) != -1 ? 2 : 1;
            for(int i = level; i <= 3; ++i)
            {
                if(lastLevel[i - 1] != -1)
                {
                    this->structureInfo->at(lastLevel[i - 1])->endBlock = textBlockIndex - 1;
                    lastLevel[i - 1] = -1;
                }
            }
            lastBlock = textBlockIndex;
            lastLevel[level - 1] = this->structureInfo->count();
            first = false;
            QString s = textBlock.text();
            stru = new FileStructureInfo;
            stru->startBlock = textBlockIndex;
            stru->endBlock = this->widgetTextEdit->document()->blockCount() - 1;
            stru->name = textBlock.text().indexOf(QRegExp("\\\\(sub){0,2}section\\{[^\\}]*\\}"), 0) != -1 ? s.replace(QRegExp(".*\\\\(sub){0,2}section\\{([^\\}]*)\\}.*"), "\\2") : "Bibliography";
            stru->level = level;
            this->structureInfo->append(stru);
        }
        textBlock = textBlock.next();
        ++textBlockIndex;
    }while(textBlockIndex < this->widgetTextEdit->document()->blockCount());

    for(int i = lastBlock; i < this->widgetTextEdit->document()->blockCount(); ++i)
    {
        this->blockIndentations[i].level = level;
        this->blockIndentations[i].next = this->widgetTextEdit->document()->blockCount();
    }

    return;

    first = true;
    textBlockIndex = 0;
    textBlock = this->widgetTextEdit->document()->begin();
    do{
        if(textBlock.text().indexOf(QRegExp("\\\\subsection\\{[^\\}]*\\}"), 0) != -1)
        {
            if(!first)
            {
                this->structureInfo->last()->endBlock = textBlockIndex-1;
            }
            level = 2;
            first = false;
            QString s = textBlock.text();
            stru = new FileStructureInfo;
            stru->startBlock = textBlockIndex;
            stru->endBlock = this->widgetTextEdit->document()->blockCount() - 1;
            stru->name = s.replace(QRegExp(".*\\\\subsection\\{([^\\}]*)\\}.*"), "\\1");
            stru->level = level;
            this->structureInfo->append(stru);
        }
        textBlock = textBlock.next();
        ++textBlockIndex;
    }while(textBlockIndex < this->widgetTextEdit->document()->blockCount());
    first = true;
    textBlockIndex = 0;
    textBlock = this->widgetTextEdit->document()->begin();
    do{
        if(textBlock.text().indexOf(QRegExp("\\\\subsubsection\\{[^\\}]*\\}"), 0) != -1)
        {
            if(!first)
            {
                this->structureInfo->last()->endBlock = textBlockIndex-1;
            }
            level = 3;
            first = false;
            QString s = textBlock.text();
            stru = new FileStructureInfo;
            stru->startBlock = textBlockIndex;
            stru->endBlock = this->widgetTextEdit->document()->blockCount() - 1;
            stru->name = s.replace(QRegExp(".*\\\\subsubsection\\{([^\\}]*)\\}.*"), "\\1");
            stru->level = level;
            this->structureInfo->append(stru);
        }
        textBlock = textBlock.next();
        ++textBlockIndex;
    }while(textBlockIndex < this->widgetTextEdit->document()->blockCount());
}
