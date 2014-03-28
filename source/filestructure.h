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

#ifndef FILESTRUCTURE_H
#define FILESTRUCTURE_H

#include <QObject>
#include <QList>

class WidgetTextEdit;

struct FileStructureInfo
{
    QString name;
    int startBlock;
    int endBlock;
    int level;
    int top;
    int height;

    FileStructureInfo() : top(0), height(0) {}
};

struct StructItem
{
    explicit StructItem() :
        parent(0),
        begin(0),
        end(0),
        blockBeginNumber(0),
        blockEndNumber(0),
        level(0) { }

    typedef enum Type {NONE, SECTION, BIBLIOGRAPHY, ENVIRONMENT} Type;


    QList<StructItem*> children;
    StructItem* parent;

    int begin;
    int end;
    int blockBeginNumber;
    int blockEndNumber;
    int level;
    Type type;
    QString name;

};

class TextStruct : public QObject
{
    Q_OBJECT
public:
    explicit TextStruct(WidgetTextEdit * parent);
    void debug();
    /**
     * @brief environmentPath return the stack of environement in wich the current cursor is in.
     * @return
     */
    QStack<const StructItem *> environmentPath() const;
    /**
     * @brief environmentPath return the stack of environement in wich the given position is in.
     * @return
     */
    QStack<const StructItem *> environmentPath(int position) const;
public slots:
    void reload();
    void clear();

private:
    void clear(StructItem * item);
    void debug(StructItem * item, int level);
    WidgetTextEdit * _widgetTextEdit;
    StructItem _environementRoot;
    StructItem _sectionRoot;
};



struct BlockIndentation
{
    int next;
    int level;
};


class FileStructure : public QObject
{
    Q_OBJECT
public:
    explicit FileStructure(WidgetTextEdit *parent = 0);
    ~FileStructure();
    QList<FileStructureInfo*> * info() { return this->structureInfo; }
    BlockIndentation *  indentations() { return this->blockIndentations; }

signals:
    
public slots:
    void updateStructure(void);
    
private:
    BlockIndentation * blockIndentations;
    QList<FileStructureInfo*> * structureInfo;
    WidgetTextEdit * widgetTextEdit;
};

#endif // FILESTRUCTURE_H
