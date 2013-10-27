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

#include "syntaxhighlighter.h"
#include <QTextCharFormat>
#include <QTextDocument>
#include <QDebug>
#include "blockdata.h"
#include "configmanager.h"
#include "widgetfile.h"
#include "widgettextedit.h"
#include "file.h"

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

void SyntaxHighlighter::highlightBlock(const QString &text)
{
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

    BlockData *blockData = new BlockData;
    if(this->previousBlockState() == 1)
    {
        blockData->insertDollar(-1);
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

    setCurrentBlockUserData(blockData);







    QTextCharFormat formatNormal = ConfigManager::Instance.getTextCharFormats("normal");
    QTextCharFormat formatCommand = ConfigManager::Instance.getTextCharFormats("command");
    QTextCharFormat formatComment = ConfigManager::Instance.getTextCharFormats("comment");
    QTextCharFormat formatMath = ConfigManager::Instance.getTextCharFormats("math");
    QTextCharFormat formatStructure = ConfigManager::Instance.getTextCharFormats("structure");


     setFormat(0, text.size(), formatNormal);


     QString patternCommand = "\\\\[a-zA-Z]+";
     QString patternStructure = "\\\\(sub){0,3}section\\{[^\\}]*\\}";

     this->highlightExpression(text,patternCommand,formatCommand);
     this->highlightExpression(text,patternStructure,formatStructure);



    int length=0;
    int lastindex = 0;
    int index=-1;
    int commentIndex = text.size();
    {
        QString patternComment =  "\\%.*$";
        QRegExp expression(patternComment);
        index = text.indexOf(expression);
        if(index >= 0)
        {
            length = expression.matchedLength();
            setFormat(index, length, formatComment);
            commentIndex = index;
        }
    }



    length=0;
    lastindex = 0;
    index=-1;



    QString mathLeftDelimiters = "\\$\\$|\\$|\\\\\\[";
    QString mathRightDelimiters = "\\$\\$|\\$|\\\\\\]";

    if(this->previousBlockState() == 1)
    {
        QString patternMathStart = "^((?!("+mathRightDelimiters+"|\\%)).)*("+mathRightDelimiters+")";
        QRegExp expression(patternMathStart);
        index = text.indexOf(expression);
        if(index >= 0 && index < commentIndex)
        {
            length = expression.matchedLength();
            setFormat(0, length, formatMath);
        }
        else
        {
            setFormat(0, text.length(), formatMath);
            this->setCurrentBlockState(1);
            return;
        }
    }

    QString patternMath = "("+mathLeftDelimiters+")((?!("+mathRightDelimiters+"|\\%)).)*("+mathRightDelimiters+")";
    lastindex = index = length;
    length = 0;
    {
        QRegExp expression(patternMath);
        index = text.indexOf(expression,index);
        while (index >= 0 && index < commentIndex) {
            lastindex = index;
            length = expression.matchedLength();
            setFormat(index, length, formatMath);
            index = text.indexOf(expression, index + length);
        }
    }
    QString patternMathEnd =  "("+mathLeftDelimiters+")((?!("+mathRightDelimiters+")).)*([\\%].*){0,1}$";
    QRegExp expression(patternMathEnd);
    index = text.indexOf(expression,lastindex+length);
    if(index >= 0 && index < commentIndex)
    {
        setFormat(index, text.length() - index, formatMath);
        this->setCurrentBlockState(1);
    }
    else
    {
        this->setCurrentBlockState(-1);
    }

    setFormat(commentIndex, text.size() - commentIndex, formatComment);

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


