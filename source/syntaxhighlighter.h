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

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QStringList>

class QTextEdit;
class WidgetFile;

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    SyntaxHighlighter(WidgetFile * widgetFile);
    ~SyntaxHighlighter();

    /**
     * @brief textBlockCommand contains command where the
     * arguments inside { } are text (and so will be check by
     * the spell checker)
     */
    static QStringList textBlockCommands;
    static QStringList otherBlockCommands;

    typedef enum State { Text, TextBlock, Other, OtherBlock, Math, Command, Comment } State;
    bool isWordSeparator(QChar c) const;
protected:
    virtual void highlightBlock(const QString &text);
    void highlightExpression(const QString &text, const QString &pattern, const QTextCharFormat &format);
private:

    WidgetFile * _widgetFile;
};

#endif // SYNTAXHIGHLIGHTER_H
