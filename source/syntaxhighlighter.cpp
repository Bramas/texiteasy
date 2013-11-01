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
    list << "cite" << "ref" << "label";
    return list;
}

QStringList SyntaxHighlighter::otherBlockCommands = initOtherBlockCommands();
QStringList SyntaxHighlighter::textBlockCommands = initTextBlockCommands();

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
        case SyntaxHighlighter::Math: return SyntaxHighlighter::Math;
        case SyntaxHighlighter::Comment: return SyntaxHighlighter::Comment;
    }
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    BlockData *blockData = new BlockData(text.length());
    setCurrentBlockUserData(blockData);

    if(_widgetFile->file()->format() == File::BIBTEX)
    {
        QTextCharFormat formatBibTitle = ConfigManager::Instance.getTextCharFormats("bibtex_command");
        QTextCharFormat formatBibKeywords = ConfigManager::Instance.getTextCharFormats("bibtex_keyword");
        QTextCharFormat formatBibString = ConfigManager::Instance.getTextCharFormats("bibtex_string");
        QTextCharFormat formatBibQuotes = ConfigManager::Instance.getTextCharFormats("bibtex_quote");
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


    int dollarPos = text.indexOf( '$' );
    while ( dollarPos != -1 )
    {
        blockData->insertDollar(dollarPos);
        dollarPos = text.indexOf( '$', dollarPos+1 );
    }


    int leftPos = text.indexOf( '{' );
    while ( leftPos != -1 )
      {
      ParenthesisInfo *info = new ParenthesisInfo;
      info->character = '{';
      info->position = leftPos;

      blockData->insertPar( info );
      leftPos = text.indexOf( '{', leftPos+1 );
      }

    int rightPos = text.indexOf('}');
    while ( rightPos != -1 )
      {
      ParenthesisInfo *info = new ParenthesisInfo;
      info->character = '}';
      info->position = rightPos;

      blockData->insertPar( info );
      rightPos = text.indexOf( '}', rightPos+1 );
      }

    leftPos = text.indexOf( "\\begin{" );
    while ( leftPos != -1 )
      {
      LatexBlockInfo *info = new LatexBlockInfo;
      info->character = 'b';
      info->position = leftPos;

      blockData->insertLat( info );
      leftPos = text.indexOf("\\begin{", leftPos+1 );
      }

    rightPos = text.indexOf("\\end{");
    while ( rightPos != -1 )
      {
      LatexBlockInfo *info = new LatexBlockInfo;
      info->character = 'e';
      info->position = rightPos;

      blockData->insertLat( info );
      rightPos = text.indexOf("\\end{", rightPos+1 );
      }




    QTextCharFormat spellingErrorFormat = ConfigManager::Instance.getTextCharFormats("normal");
    spellingErrorFormat.setFontUnderline(true);
    spellingErrorFormat.setUnderlineColor(QColor(Qt::red));
    spellingErrorFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);



    QTextCharFormat formatNormal = ConfigManager::Instance.getTextCharFormats("normal");
    QTextCharFormat formatCommand = ConfigManager::Instance.getTextCharFormats("command");
    QTextCharFormat formatCommandInMathMode = ConfigManager::Instance.getTextCharFormats("command-in-math-mode");
    QTextCharFormat formatComment = ConfigManager::Instance.getTextCharFormats("comment");
    QTextCharFormat formatMath = ConfigManager::Instance.getTextCharFormats("math");
    QTextCharFormat formatStructure = ConfigManager::Instance.getTextCharFormats("structure");


     setFormat(0, text.size(), formatNormal);


State state = intToState(previousBlockState());
 State previousState = state;

int index = 0;
QChar currentChar;
QChar nextChar;
QString commandBuffer;
qDebug()<<"previous state "<<state;
while(index < text.length())
{
    qDebug()<<state;
    currentChar = text.at(index);
    if(index < text.length() - 1)
    {
        nextChar = text.at(index + 1);
    }
    else
    {
        nextChar == QChar::Null;
    }
    // if the end of line is commented, we break the loop and we keep the current state (we do not save state = Comment)
    if(currentChar == '%')
    {
        setFormat(index, text.size() - index, formatComment);
        for(int comment_idx = index; comment_idx < text.size(); ++comment_idx)
        {
            blockData->state[comment_idx] = Comment;
        }
        break;
    }
    if(currentChar == '{')
    {
        blockData->parenthesisLevel.top() += 1;
    }
    else if(currentChar == '}')
    {
        blockData->parenthesisLevel.top() -= 1;
    }
    switch(state)
    {
    case Text:
        if(currentChar != ' ' && currentChar != '\t' && blockData->parenthesisLevel.top() == 0 && blockData->parenthesisLevel.count() > 1)
        {
            state = previousState;
            blockData->parenthesisLevel.pop();
        }
        else
        if(      currentChar == '$'
             ||  currentChar == '\\' && nextChar == '[')
        {
            state = Math;
            setFormat(index, 1, formatMath);
            if(currentChar == '$' && nextChar == '$')
            {
                setFormat(index + 1, 1, formatMath);
                ++index;
            }
        }
        else
        if(currentChar == '\\')
        {
            previousState = Text;
            state = Command;
            commandBuffer = QString::null;
            setFormat(index, 1, formatCommand);
        }
        break;
    case TextBlock:
        if(      currentChar == ' '
             ||  currentChar == '\t')
        {
            ++index;
        }
        else
        if(currentChar != '{')
        {
            state = Text;
            --index;
        }
        else
        {
            state = Text;
        }
        break;
    case Other:
        if(currentChar != ' ' && currentChar != '\t' && blockData->parenthesisLevel.top() == 0)
        {
            blockData->parenthesisLevel.pop();
            state = previousState;
        }
        /*else
        if(      currentChar == '$'
             ||  currentChar == '\\' && nextChar == '[')
        {
            previousState = Other;
            state = Math;
            setFormat(index, 1, formatMath);
            if(currentChar == '$' && nextChar == '$')
            {
                setFormat(index + 1, 1, formatMath);
                ++index;
            }
        }
        else
        if(currentChar == '\\')
        {
            previousState = Other;
            state = Command;
            commandBuffer = QString::null;
            setFormat(index, 1, formatCommand);
        }*/
        else
        {
            state = Other;
        }
        break;
    case OtherBlock:
        if(      currentChar == ' '
             ||  currentChar == '\t')
        {
            //nothing
        }
        else
        if(currentChar != '{')
        {
            state = previousState;
            --index;
        }
        else
        {
            blockData->parenthesisLevel.top() -= 1;
            blockData->parenthesisLevel.push(1);
            state = Other;
        }
        break;
    case Math:
        setFormat(index, 1, formatMath);
        if(   currentChar == '\\' && nextChar == ']'
           || currentChar == '$' && nextChar == '$')
        {
            state = Text;
            setFormat(index + 1, 1, formatMath);
            ++index;
        }
        else
        if(currentChar == '$')
        {
            state = Text;
        }
        else
        if(currentChar == '\\')
        {
            previousState = Math;
            state = Command;
            commandBuffer = QString::null;
            setFormat(index, 1, formatCommandInMathMode);
        }
        break;
    case Command:
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
            blockData->state[index] = state;
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
            if(textBlockCommands.contains(commandBuffer))
            {
                blockData->parenthesisLevel.push(0);
                state = Text;
                --index;
            }
            else
            if(otherBlockCommands.contains(commandBuffer))
            {
                /*if(currentChar == ' ' || currentChar == '\t')
                {
                    state = OtherBlock;
                }
                else
                {
                    blockData->parenthesisLevel.top() -= 1;
                    blockData->parenthesisLevel.push(1);
                    state = Other;
                }*/
                blockData->parenthesisLevel.push(0);
                state = Other;
                --index;
            }
            else
            {
                if(previousState == Text)
                {
                    state = Text;
                }
                else
                {
                    state = Math;
                    setFormat(index, 1, formatMath);
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
    blockData->state[index] = state;
    ++index;
    if(blockData->parenthesisLevel.top() < 0)
    {
        blockData->parenthesisLevel.top() = 0;
    }
}


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
        while ((blockData->state[i] == Text) && (!isWordSeparator(ch)))
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
                    blockData->misspelled[i - buffer.length() + buffer_idx] = true;
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
setCurrentBlockState(state);

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
/*    switch (c.toLatin1()) {
    case '.':
    case ',':
    case '?':
    case '!':
    case ':':
    case ';':
    case '+':
    case '<':
    case '>':
    case '[':
    case ']':
    case '(':
    case ')':
    case '{':
    case '}':
    case '=':
    case '/':
    case '+':
    case '%':
    case '&':
    case '^':
    case '`':
    case '*':
    case '_':
    case '\\':
    case '\'':
    case '\t':
    case '\n':
    case '"':
    case '~':
    case '$':
    case '|':
    case '#':
    case '£':
    case '@':
    case 'µ':
    case '=':
    case '¨':
        return true;
    default:
        return false;
    }*/
    return QString(c).contains(QRegExp(QString::fromUtf8("[^a-zâãäåæçèéêëìíîïðñòóôõøùúûüýþÿı]"),Qt::CaseInsensitive));
}

