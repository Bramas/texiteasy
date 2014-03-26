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

#ifndef BLOCKDATA_H
#define BLOCKDATA_H

#include <QTextBlock>
#include <QStack>
#include <QTextBlockUserData>
#include <QPointer>

struct ParenthesisInfo {
    QChar character;
    int position;
};

struct LatexBlockInfo {

    typedef enum Type {NONE, ENVIRONEMENT_BEGIN, ENVIRONEMENT_END, BIBLIOGRAPHY_BEGIN, BIBLIOGRAPHY_END, SECTION} Type;


    Type type;
    int position;
    int sectionLevel;
    QString name;
};

class BlockData;
class BlockState
{
public:
    BlockState() { state = 0; stateAfterOption = 0; parenthesisLevel.push(0); crocherLevel.push(0); }
    BlockState(int state, int previousState, int stateAfterOption);
    bool equals(const BlockState & other) const;
    int state;
    int previousState;
    int stateAfterOption;
    QStack<int> parenthesisLevel;
    QStack<int> crocherLevel;

};

class CharacterData
{
public:
    CharacterData() : misspelled(false), state(0) {}
    bool misspelled;
    int state;
};

class BlockData : public QTextBlockUserData
{

public:
    BlockData(int length = 1);
    ~BlockData();
    static BlockData *data(const QTextBlock &block) { return static_cast<BlockData *>(block.userData()); }

    QVector<CharacterData> characterData;
    QVector<ParenthesisInfo *> parentheses();
    QVector<LatexBlockInfo *> latexblocks();
    QVector<QPair<QString,QPair<int,int> > > arguments;
    void insertPar( ParenthesisInfo *info );
    void insertLat( LatexBlockInfo *info );
    void insertDollar(int pos ) { this->_dollars.append(pos); }
    bool isAClosingDollar(int position);
    int length() { return _length; }
    BlockState blockStartingState;
    BlockState blockEndingState;
private:
    QVector<ParenthesisInfo *> _parentheses;
    QVector<LatexBlockInfo *> _latexblocks;
    QVector<int> _dollars;
    int _length;
};


#endif // BLOCKDATA_H
