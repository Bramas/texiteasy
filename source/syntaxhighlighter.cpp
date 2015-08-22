/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/

#include "hunspell/hunspell.hxx"

#include "syntaxhighlighter.h"
#include <QTextCharFormat>
#include <QTextDocument>
#include <QDebug>
#include <QTextCodec>
#include "blockdata.h"
#include "configmanager.h"
#include "widgetfile.h"
#include "widgettextedit.h"
#include "file.h"

#include <QBrush>

QStringList initTextBlockCommands()
{
    QStringList list;
    list << "text"
    << "textbf"
    << "textit"
    << "textrm"
    << "emph"
    << "section" << "subsection" << "subsubsection" << "paragraph";
    return list;
}

QStringList initOtherBlockCommands()
{
    QStringList list;
    list << "cite" << "ref" << "label" << "begin" << "end" << "input" << "includegraphics"
         << "bibliographystyle" << "bibliography" << "usepackage" << "newcommand" << "renewcommand"
         << "newtheorem" << "bibitem" << "hypersetup";
    return list;
}
QStringList initCommandsWithOptions()
{
    QStringList list;
    list << "usepackage" << "newcommand" << "includegraphics" << "begin"
         << "item";
    return list;
}
QStringList initMathEnvironment()
{
    QStringList list;
    list << "displaymath" << "equation" ;
    return list;
}

QStringList SyntaxHighlighter::otherBlockCommands = initOtherBlockCommands();
QStringList SyntaxHighlighter::textBlockCommands = initTextBlockCommands();
QStringList SyntaxHighlighter::commandsWithOptions = initCommandsWithOptions();
QStringList SyntaxHighlighter::mathEnvironments = initMathEnvironment();

SyntaxHighlighter::SyntaxHighlighter(WidgetFile *widgetFile) :
    QSyntaxHighlighter(widgetFile->widgetTextEdit()->document())
{
    _widgetFile = widgetFile;

}
SyntaxHighlighter::~SyntaxHighlighter()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete SyntaxHighlighter";
#endif
}


SyntaxHighlighter::State intToState(int in) {
    switch(in) {
        default:
        case SyntaxHighlighter::Text: return SyntaxHighlighter::Text;
        case SyntaxHighlighter::Other: return SyntaxHighlighter::Other;
        case SyntaxHighlighter::Option: return SyntaxHighlighter::Option;
        case SyntaxHighlighter::Math: return SyntaxHighlighter::Math;
        case SyntaxHighlighter::Comment: return SyntaxHighlighter::Comment;
    }
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    //qDebug()<<"begin highlight block "<<currentBlock().blockNumber();
    BlockData *blockData = new BlockData(text.length());
    setCurrentBlockUserData(blockData);
    QTextBlock previousBlock = currentBlock().previous();
    if(previousBlock.isValid())
    {
        BlockData * previousData = static_cast<BlockData *>(previousBlock.userData());
        if(previousData)
        {
            blockData->blockStartingState = previousData->blockEndingState;
            blockData->blockEndingState = blockData->blockStartingState;
        }
    }

    if(_widgetFile->file()->format() == File::BIBTEX)
    {
        QTextCharFormat formatBibTitle = ConfigManager::Instance.getTextCharFormats("bibtex/command");
        QTextCharFormat formatBibKeywords = ConfigManager::Instance.getTextCharFormats("bibtex/keyword");
        QTextCharFormat formatBibString = ConfigManager::Instance.getTextCharFormats("bibtex/string");
        QTextCharFormat formatBibQuotes = ConfigManager::Instance.getTextCharFormats("bibtex/quote");
        QTextCharFormat formatComment = ConfigManager::Instance.getTextCharFormats("comment");


        QString patternBibTitle = "@[a-zA-Z\\-_]+";
        QString patternBibString = "\\\"[^\\\"]+\\\"";
        QString patternBibQuotes = "\\\"";
        QString patternBibKeywords = "(author|title|year|publisher|booktitle|editor|series|volume|isbn|bibsource|number|pages|ee|journal|crossref)[ ]*=";
        this->highlightExpression(text,patternBibTitle,formatBibTitle);
        this->highlightExpression(text,patternBibKeywords, formatBibKeywords);
        this->highlightExpression(text,patternBibString, formatBibString);
        this->highlightExpression(text,patternBibQuotes, formatBibQuotes);


        int commentIndex = text.size();
        {
            QString patternComment =  "\\%.*$";
            QRegExp expression(patternComment);
            int index = text.indexOf(expression);
            int length=0;
            if(index >= 0)
            {
                length = expression.matchedLength();
                setFormat(index, length, formatComment);
                commentIndex = index;
            }
        }
        setFormat(commentIndex, text.size() - commentIndex, formatComment);

        return;
    }



    QTextCharFormat spellingErrorFormat = ConfigManager::Instance.getTextCharFormats("normal");
    spellingErrorFormat.setFontUnderline(true);
    spellingErrorFormat.setUnderlineColor(QColor(Qt::red));
    spellingErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);



    QTextCharFormat formatNormal = ConfigManager::Instance.getTextCharFormats("normal");
    QTextCharFormat formatCommand = ConfigManager::Instance.getTextCharFormats("command");
    QTextCharFormat formatCommandInMathMode = ConfigManager::Instance.getTextCharFormats("command-in-math-mode");
    QTextCharFormat formatOption = ConfigManager::Instance.getTextCharFormats("option");
    QTextCharFormat formatComment = ConfigManager::Instance.getTextCharFormats("comment");
    QTextCharFormat formatMath = ConfigManager::Instance.getTextCharFormats("math");
    QTextCharFormat formatOther = ConfigManager::Instance.getTextCharFormats("other");
    QTextCharFormat formatVerbatim = formatOther;
    QTextCharFormat formatArgument;
    QTextCharFormat formatArgumentDelimiter = formatArgument;
#if QT_VERSION > 0x050000
                    formatArgumentDelimiter.setFontStretch(1);
#endif
                    formatArgumentDelimiter.setFontLetterSpacing(10);
                    formatArgumentDelimiter.setForeground(QBrush(QColor(0,0,0,0)));

     setFormat(0, text.size(), formatNormal);


QString currentEnvironment = blockData->blockStartingState.environment;
State state = intToState(blockData->blockStartingState.state);
State previousState = intToState(blockData->blockStartingState.previousState);
State stateAfterOption = intToState(blockData->blockStartingState.stateAfterOption);
State stateAfterArguments = intToState(blockData->blockStartingState.stateAfterArguments);
bool beginEnvironmentName = false;
bool endEnvironmentName = false;
bool waitingBraceArgument = false;
QStack<int> * parenthesisLevel = &(blockData->blockEndingState.parenthesisLevel);
QStack<int> * crocherLevel = &(blockData->blockEndingState.crocherLevel);

int index = 0;
QChar currentChar;
QChar nextChar;
QChar verbCharacter;
QString commandBuffer;
QString environmentNameBuffer;
QString endEnvironmentNameBuffer;
bool setCharacterState;
bool escapedChar;
int overrideCurrentState;
//qDebug()<<"previous state "<<state;
while(index < text.length())
{
    currentChar = text.at(index);
    overrideCurrentState = -1;
    setCharacterState = true;
    //qDebug()<<index<<" : "<<currentChar<<" state : "<<state<<", crocherlevel : "<<(*crocherLevel);
    if(index < text.length() - 1)
    {
        nextChar = text.at(index + 1);
    }
    else
    {
        nextChar = QChar::Null;
    }
    if((state == Command && commandBuffer.isNull()) || state == Verbatim || (state == Command && !commandBuffer.compare("verb") && QString(currentChar).contains(QRegExp("[^a-zA-Z]"))))
    {
        escapedChar = true;
    }
    else
    {
        escapedChar = false;
    }
    // if the end of line is commented, we break the loop and we keep the current state (we do not save state = Comment)
    if(currentChar == '\\')
    {
        if(index + 7 < text.length())
        {    //\\\\verb\\#\\{\\{([^\\}]*)\\}\\}\\#
            if(      text.at(index+1) == 'v'
                  && text.at(index+2) == 'e'
                  && text.at(index+3) == 'r'
                  && text.at(index+4) == 'b'
                  && text.at(index+5) == '#'
                  && text.at(index+6) == '{'
                  && text.at(index+7) == '{')
            {
                int tmp = index + 7;
                QString argument;
                while(tmp + 1 < text.length() && QString(text.at(tmp + 1)).contains(QRegExp("[a-zA-Z0-9 ]")))
                {
                    argument += text.at(tmp + 1);
                    ++tmp;
                }
                if(tmp + 3 < text.length() && text.at(tmp+1) == '}' && text.at(tmp+2) == '}' && text.at(tmp+3) == '#')
                {
                    // only here we know that it is an argument.
                    setFormat(index, 8, formatArgumentDelimiter);
                    for(int idx = index; idx < tmp + 4; ++idx)
                    {
                        blockData->characterData[idx].state = CompletionArgument;
                    }
                    setFormat(index + 8, argument.length(), formatArgument);
                    setFormat(tmp + 1, 3, formatArgumentDelimiter);
                    blockData->arguments.append(QPair<QString,QPair<int,int> >(argument,QPair<int,int>(index, tmp + 3)));
                    index = tmp + 4;
                    continue;
                }
            }
        }
    }
    if(currentChar == '%' && !escapedChar)
    {
        setFormat(index, text.size() - index, formatComment);
        for(int comment_idx = index; comment_idx < text.size(); ++comment_idx)
        {
            blockData->characterData[comment_idx].state = Comment;
        }
        break;
    }
    if(currentEnvironment == "comment")
    {
        int end = text.indexOf(QRegExp("\\\\end\\{comment\\}"), index);
        if(end == -1)
        {
            setFormat(index, text.size() - index, formatComment);
            for(int comment_idx = index; comment_idx < text.size(); ++comment_idx)
            {
                blockData->characterData[comment_idx].state = Comment;
            }
            break;
        }
        else
        {
            setFormat(index, end, formatComment);
            for(int comment_idx = index; comment_idx < end; ++comment_idx)
            {
                blockData->characterData[comment_idx].state = Comment;
            }
            index = end;
            currentEnvironment = "";
            continue;
        }
    }

    if(currentChar == '{' && !escapedChar)
    {
        waitingBraceArgument = false;
        parenthesisLevel->top() += 1;
    }
    if(waitingBraceArgument)
    {
        if(currentChar != ' ' && currentChar != '\t' && (currentChar != '{' || escapedChar) && parenthesisLevel->top() == 0
                && parenthesisLevel->count() > 1)
        {
            state = previousState;
            waitingBraceArgument = false;
            parenthesisLevel->pop();
            index = index - 1 < 0 ? 0 : index - 1;
            continue;
        }
        else //if(currentChar != ' ' && currentChar != '\t' && parenthesisLevel->pop())
        {

        }
    }
    if(currentChar == '{' && !escapedChar){}
    else if(currentChar == '}' && !escapedChar)
    {
        parenthesisLevel->top() -= 1;
    }
    if(currentChar == '[' && !escapedChar)
    {
        crocherLevel->top() += 1;
    }
    else if(currentChar == ']' && !escapedChar)
    {
        crocherLevel->top() -= 1;
    }
    //qDebug()<<currentChar<<" "<<state<<" "<<parenthesisLevel->top()<< " ("<<waitingBraceArgument<<")";

    switch(state)
    {
    case Comment:
    case CompletionArgument:
        break;
    case Text:
        if(currentChar != ' ' && currentChar != '\t' && parenthesisLevel->top() == 0 && parenthesisLevel->count() > 1)
        {
            state = Math;
            parenthesisLevel->pop();
        }
        else
        if(      (currentChar == '$'  && !escapedChar)
             ||  ((currentChar == '\\' && !escapedChar) && nextChar == '['))
        {
            state = Math;
            setFormat(index, 1, formatMath);
            if(currentChar == '$' && !escapedChar && nextChar == '$')
            {
                setFormat(index + 1, 1, formatMath);
                ++index;
            }
        }
        else
        if(currentChar == '\\' && !escapedChar)
        {
            previousState = Text;
            state = Command;
            commandBuffer = QString::null;
            setFormat(index, 1, formatCommand);
        }
        break;
    case Other:
        if(currentChar != ' ' && currentChar != '\t' && parenthesisLevel->top() == 0)
        {
            setFormat(index, 1, formatOther);
            if(currentChar=='{')
            {
                waitingBraceArgument = true;
            }
            else
            {
                state = stateAfterArguments;
                if(parenthesisLevel->count() >= 2)
                {
                    parenthesisLevel->pop();
                }
            }
            if(!environmentNameBuffer.isEmpty())
            {
                currentEnvironment = environmentNameBuffer;
                environmentNameBuffer = "";
                if(mathEnvironments.contains(currentEnvironment))
                {
                    state = Math;
                }
            }
            else
            if(!endEnvironmentNameBuffer.isEmpty())
            {
                if(mathEnvironments.contains(endEnvironmentNameBuffer))// && endEnvironmentNameBuffer == currentEnvironment)
                {
                    state = Text;
                    overrideCurrentState = Math;
                    index -= QString("\\end{"+endEnvironmentNameBuffer+"}").size();
                    currentEnvironment = "";
                    // don't clean endEnvironmentNameBuffer here so that the second time the end environement is processed, it will not enter again this condition (avoiding an ininite loop)
                }
                else
                {
                    endEnvironmentNameBuffer = "";
                }
            }

        }
        else
        {
            if(!environmentNameBuffer.isEmpty() || (beginEnvironmentName && currentChar != ' ' && currentChar != '{' && currentChar != '['))
            {
                beginEnvironmentName = false;
                environmentNameBuffer += currentChar;
            }
            if(!endEnvironmentNameBuffer.isEmpty() || (endEnvironmentName && currentChar != ' ' && currentChar != '{' && currentChar != '['))
            {
                endEnvironmentName = false;
                endEnvironmentNameBuffer += currentChar;
            }
            setFormat(index, 1, formatOther);
            state = Other;
        }
        break;
    case Math:
        setFormat(index, 1, formatMath);
        if(   (currentChar == '\\' && !escapedChar && nextChar == ']')
           || (currentChar == '$' && !escapedChar && nextChar == '$'))
        {
            state = Text;
            overrideCurrentState = Math;
            setFormat(index + 1, 1, formatMath);
            ++index;
        }
        else
        if(currentChar == '$' && !escapedChar)
        {
            state = Text;
            overrideCurrentState = Math;
        }
        else
        if(currentChar == '\\' && !escapedChar)
        {
            previousState = Math;
            state = Command;
            commandBuffer = QString::null;
            setFormat(index, 1, formatCommandInMathMode);
        }
        break;
    case Option:
        if(currentChar != ' ' && currentChar != '\t' && crocherLevel->top() == 0)
        {
            crocherLevel->pop();
            if(currentChar == '{' && !escapedChar)
            {
                parenthesisLevel->top() -= 1;
            }
            if(stateAfterOption != previousState)
            {
                parenthesisLevel->push(0);
            }
            if(currentChar == ']' && !escapedChar)
            {
                setFormat(index, 1, formatOption);
            }
            else
            {
                --index;
                setCharacterState = false;
            }
            state = stateAfterOption;
            waitingBraceArgument = true;
        }
        else
        {
            setFormat(index, 1, formatOption);
            state = Option;
        }
        break;
    case Verbatim:
        if(currentChar == verbCharacter)
        {
            setFormat(index, 1, formatVerbatim);
            state = previousState;
            overrideCurrentState = Verbatim;
        }
        else
        {
            setFormat(index, 1, formatVerbatim);
        }
        break;
    case Command:
        if(commandBuffer.isNull() && QString(currentChar).contains(QRegExp("[^a-zA-Z]")))
        {
            if(previousState == Math)
            {
                setFormat(index, 1, formatCommandInMathMode);
            }else
            {
                setFormat(index, 1, formatCommand);
            }
            state = previousState;
            break;
        }
        if(currentChar == '*')
        {
            if(previousState == Text)
            {
                setFormat(index, 1, formatCommand);
            }
            else
            {
                setFormat(index, 1, formatCommandInMathMode);
            }
            //go to the next index;
            blockData->characterData[index].state = state;
            ++index;
            if(index < text.length())
            {
                currentChar = text.at(index);
            }
            else
            {
                break;
            }
        }
        if(QString(currentChar).contains(QRegExp("[^a-zA-Z]")))
        {
            if(!commandBuffer.compare("verb"))
            {
                setFormat(index, 1, formatVerbatim);
                verbCharacter = currentChar;
                state = Verbatim;
                break;
            }
            if(!commandBuffer.compare("begin"))
            {
                beginEnvironmentName = true;
            }
            if(!commandBuffer.compare("end"))
            {
                endEnvironmentName = true;
            }
            if(currentChar == ']')
            {
                crocherLevel->top() += 1;
                --index;
                setCharacterState = false;
                state = previousState;
                break;
            }
            if(currentChar == '}')
            {
                parenthesisLevel->top() += 1;
                --index;
                setCharacterState = false;
                state = previousState;
                break;
            }
            if(currentChar == '[')
            {
                crocherLevel->top() -= 1;
            }else
            if(currentChar == '{')
            {
                parenthesisLevel->top() -= 1;
            }
            if(textBlockCommands.contains(commandBuffer))
            {
                crocherLevel->push(0);
                state = Option;
                stateAfterOption = Text;
                stateAfterArguments = previousState;
                --index;
                setCharacterState = false;
            }
            else
            if(otherBlockCommands.contains(commandBuffer))
            {
                crocherLevel->push(0);
                state = Option;
                stateAfterArguments = previousState;
                stateAfterOption = Other;
                --index;
                setCharacterState = false;
            }
            else
            {
                if(previousState == Text)
                {
                    crocherLevel->push(0);
                    state = Option;
                    stateAfterOption = Text;
                    --index;
                    setCharacterState = false;
                }
                else
                {
                    crocherLevel->push(0);
                    state = Option;
                    stateAfterOption =  Math;
                    --index;
                    setCharacterState = false;
                }
            }
            if(!commandsWithOptions.contains(commandBuffer))
            {
                crocherLevel->pop();
                state = stateAfterOption;
                if(state == Other)
                {
                    waitingBraceArgument = true;
                }
                if(state != previousState)
                {
                    parenthesisLevel->push(0);
                }
            }
        }
        else
        {
            commandBuffer += currentChar;
            if(previousState == Text)
            {
                setFormat(index, 1, formatCommand);
            }
            else
            {
                setFormat(index, 1, formatCommandInMathMode);
            }
        }
        break;
    }
    if(parenthesisLevel->size() == 0)
    {
        parenthesisLevel->push(0);
    }
    if(parenthesisLevel->top() < 0)
    {
        parenthesisLevel->top() = 0;
    }
    if(crocherLevel->top() < 0)
    {
        crocherLevel->top() = 0;
    }

    if(index < 0)
    {
        index = 0;
    }
    else
    {
        if(index >= text.length())
        {
            break;
        }
        //qDebug()<<index<<" "<<currentChar<<" ste="<<setCharacterState<<" state="<<state<<" o="<<overrideCurrentState;
        if(setCharacterState)
        {
            if(overrideCurrentState != -1)
            {
                blockData->characterData[index].state = overrideCurrentState;
            }
            else
            {
                blockData->characterData[index].state = state;
            }
        }
        ++index;
    }
}




//*****************************************************************************
//



int dollarPos = text.indexOf( '$' );
while ( dollarPos != -1 )
{
    if(   blockData->characterData[dollarPos].state != Verbatim
       && blockData->characterData[dollarPos].state != Command)
    {
        blockData->insertDollar(dollarPos);
    }
    dollarPos = text.indexOf( '$', dollarPos+1 );
}


int leftPos;/* = text.indexOf( '{' );
while ( leftPos != -1 )
{
    if(   blockData->characterData[leftPos].state != Verbatim
       && blockData->characterData[leftPos].state != Command)
    {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->type = ParenthesisInfo::LEFT_BRACE;
        info->position = leftPos;

        blockData->insertPar( info );
    }
  leftPos = text.indexOf( '{', leftPos+1 );
}

leftPos = text.indexOf( '[' );
while ( leftPos != -1 )
{
   if(   blockData->characterData[leftPos].state != Verbatim
      && blockData->characterData[leftPos].state != Command)
   {
       ParenthesisInfo *info = new ParenthesisInfo;
       info->type = ParenthesisInfo::LEFT_CROCHET;
       info->position = leftPos;

       blockData->insertPar( info );
   }
 leftPos = text.indexOf( '[', leftPos+1 );
}
*/

QRegExp leftPattern("(\\\\left[\\\\]{0,1}[^a-zA-Z\\\\]|\\{|\\[|\\()");
leftPos = text.indexOf( leftPattern );
while ( leftPos != -1 )
{
    if(   blockData->characterData[leftPos].state != Verbatim
      && blockData->characterData[leftPos].state != Comment)
    {
    ParenthesisInfo *info = new ParenthesisInfo;
    QString left = leftPattern.capturedTexts().at(1);
    info->position = leftPos;
    info->length = leftPattern.matchedLength();
    if(!left.compare("{"))
    {
        info->type = ParenthesisInfo::LEFT_BRACE;
    }
    else if(!left.compare("["))
    {
        info->type = ParenthesisInfo::LEFT_CROCHET;
    }
    else if(!left.compare("("))
    {
        info->type = ParenthesisInfo::LEFT_PARENTHESIS;
    }
    else
    {
        info->type = ParenthesisInfo::LEFT;
        //info->length += 2;
    }
    leftPos += info->length;

    blockData->insertPar( info );
    }
    else
    {
        leftPos += leftPattern.matchedLength();
    }
 leftPos = text.indexOf( leftPattern, leftPos );
}
QRegExp rightPattern("(\\\\right[\\\\]{0,1}[^a-zA-Z\\\\]|\\}|\\]|\\))");
int rightPos = text.indexOf( rightPattern );
while ( rightPos != -1 )
{
    if(   blockData->characterData[rightPos].state != Verbatim
          && blockData->characterData[rightPos].state != Comment
          )
    {
        ParenthesisInfo *info = new ParenthesisInfo;
        QString right = rightPattern.capturedTexts().at(1);
        info->length = rightPattern.matchedLength();
        info->position = rightPos;
        if(!right.compare("}"))
        {
            info->type = ParenthesisInfo::RIGHT_BRACE;
        }
        else if(!right.compare("]"))
        {
            info->type = ParenthesisInfo::RIGHT_CROCHET;
        }
        else if(!right.compare(")"))
        {
            info->type = ParenthesisInfo::RIGHT_PARENTHESIS;
        }
        else
        {
            info->type = ParenthesisInfo::RIGHT;
            //info->length++;
        }
        rightPos += info->length;
        blockData->insertPar( info );
    }
    else
    {
        rightPos += rightPattern.matchedLength();
    }
    rightPos = text.indexOf( rightPattern, rightPos);
}

/*

int rightPos = text.indexOf('}');
while ( rightPos != -1 )
{
    if(   blockData->characterData[rightPos].state != Verbatim
       && blockData->characterData[rightPos].state != Command)
    {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->type = ParenthesisInfo::RIGHT_BRACE;
        info->position = rightPos;

        blockData->insertPar( info );
    }
    rightPos = text.indexOf( '}', rightPos+1 );
}
rightPos = text.indexOf(']');
while ( rightPos != -1 )
{
    if(   blockData->characterData[rightPos].state != Verbatim
       && blockData->characterData[rightPos].state != Command)
    {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->type = ParenthesisInfo::RIGHT_CROCHET;
        info->position = rightPos;

        blockData->insertPar( info );
    }
    rightPos = text.indexOf( ']', rightPos+1 );
}
*/


QRegExp beginPattern("\\\\begin\\{([^\\}]*)\\}");
leftPos = text.indexOf( beginPattern );
while ( leftPos != -1 )
{
    if(blockData->characterData[leftPos].state != Verbatim &&
            blockData->characterData[leftPos].state != Comment)
    {
        LatexBlockInfo *info = new LatexBlockInfo;
        info->type        = LatexBlockInfo::ENVIRONEMENT_BEGIN;
        info->position    = leftPos;
        info->blockNumber = currentBlock().blockNumber();
        info->name        = beginPattern.capturedTexts().at(1);

        if(!info->name.compare("document"))
        {
            LatexBlockInfo *infoSection = new LatexBlockInfo;
            infoSection->type         = LatexBlockInfo::SECTION;
            infoSection->position     = info->position;
            infoSection->blockNumber  = info->blockNumber;
            infoSection->sectionLevel = LatexBlockInfo::LEVEL_DOCUMENT;
            infoSection->name         = "Document";
            blockData->insertLat( infoSection );
        }
        blockData->insertLat( info );
    }
    leftPos = text.indexOf(beginPattern, leftPos+1 );
}

QRegExp endPattern("\\\\end\\{([^\\}]*)\\}");
rightPos = text.indexOf(endPattern);
while ( rightPos != -1 )
{
    if(blockData->characterData[rightPos].state != Verbatim &&
            blockData->characterData[rightPos].state != Comment)
    {
        LatexBlockInfo *info = new LatexBlockInfo;
        info->type        = LatexBlockInfo::ENVIRONEMENT_END;
        info->position    = rightPos + endPattern.matchedLength() + 1;
        info->blockNumber = currentBlock().blockNumber();
        info->name        = endPattern.capturedTexts().at(1);

        blockData->insertLat( info );
    }
    rightPos = text.indexOf(endPattern, rightPos+1 );
}

QRegExp sectionsPattern("\\\\((sub)*)(chapter|paragraph|section)\\{([^\\}]*)\\}");
rightPos = text.indexOf(sectionsPattern);
while ( rightPos != -1 )
{
    LatexBlockInfo *info = new LatexBlockInfo;
    info->type          = LatexBlockInfo::SECTION;
    info->position      = rightPos;
    info->blockNumber   = currentBlock().blockNumber();
    info->name          = sectionsPattern.capturedTexts().at(4);
    QString sectionType(sectionsPattern.capturedTexts().at(3));
    if(!sectionType.compare("section"))
    {
        int subLength = sectionsPattern.capturedTexts().at(1).length();
        switch(int(subLength/3))
        {
        default:
        case 0:
            info->sectionLevel = LatexBlockInfo::LEVEL_SECTION;
            break;
        case 1:
            info->sectionLevel = LatexBlockInfo::LEVEL_SUBSECTION;
            break;
        case 2:
            info->sectionLevel = LatexBlockInfo::LEVEL_SUBSECTION;
            break;
        }
    }
    else if(!sectionType.compare("chapter"))
    {
        info->sectionLevel = LatexBlockInfo::LEVEL_CHAPTER;
    }
    else
    {
        info->sectionLevel = LatexBlockInfo::LEVEL_PARAGRAPH;
    }

  blockData->insertLat( info );
  rightPos = text.indexOf(sectionsPattern, rightPos+1 );
}









//*****************************************************************************
// Spell Checker

if (_widgetFile->spellChecker())
{
    QString buffer;
    QChar ch;
    int i=0;
    int check;
    QByteArray encodedString;
    QTextCodec *codec = QTextCodec::codecForName(_widgetFile->spellCheckerEncoding().toLatin1());

    while (i < text.length())
    {
        buffer = QString::null;
        ch = text.at( i );
        while ((blockData->characterData[i].state == Text) && (!isWordSeparator(ch)))
        {
              buffer += ch;
              i++;
              if (i < text.length()) ch = text.at( i );
              else break;
        }
        if ((buffer.length() > 1))// && (!ignoredwordList.contains(buffer)) && (!hardignoredwordList.contains(buffer)))
        {
            encodedString = codec->fromUnicode(buffer);
            check = _widgetFile->spellChecker()->spell(encodedString.data());
            if (!check)
            {
                for(int buffer_idx = 0; buffer_idx < buffer.length(); ++buffer_idx)
                {
                    QTextCharFormat f = format(i - buffer.length() + buffer_idx);
                    f.setFontUnderline(true);
                    f.setUnderlineColor(QColor(Qt::red));
                    setFormat(i - buffer.length() + buffer_idx, 1, f);
                    blockData->characterData[i - buffer.length() + buffer_idx].misspelled = true;
                }
            }
        }
        i++;
    }
}


//DO NOT set current block state to Command -> this may cause out of range index in array. (see case Command in the main switch above)
if(state == Command)
{
    state = previousState;
}

blockData->blockEndingState.state               = state;
blockData->blockEndingState.previousState       = previousState;
blockData->blockEndingState.stateAfterOption    = stateAfterOption;
blockData->blockEndingState.stateAfterArguments    = stateAfterArguments;
blockData->blockEndingState.environment    = currentEnvironment;

// Check if we need to rehighlight the next block
QTextBlock nextBlock = this->currentBlock().next();
if(!nextBlock.isValid())
{
    if(parenthesisLevel->count() > 1 || parenthesisLevel->top() != 0)
    {
        QTextCharFormat f;
        f.setForeground(QBrush(QColor(255,0,0)));
        setFormat(0,currentBlock().length(), f);
    }

}
if(nextBlock.isValid())
{
    BlockData * nextData = static_cast<BlockData *>(nextBlock.userData());
    BlockState nextStartingData;
    if(nextData && (nextStartingData = nextData->blockStartingState).state > -1)
    {
        if(!nextStartingData.equals(blockData->blockEndingState))
        {
            //change the currentBlock state to request the update of the next block
            setCurrentBlockState(-currentBlockState());
        }

    }
}
//qDebug()<<"end highlight block";
}
void SyntaxHighlighter::highlightExpression(const QString &text, const QString &pattern, const QTextCharFormat &format)
{
    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, format);
        index = text.indexOf(expression, index + length);
    }
}

bool SyntaxHighlighter::isWordSeparator(QChar c) const
{
    return QString(c).contains(QRegExp(QString::fromUtf8("[^a-zâãäåàæçèéêëìíîïðñòóôõøùúûüýþÿıœ]"),Qt::CaseInsensitive));
}

