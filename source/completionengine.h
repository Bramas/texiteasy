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

#ifndef COMPLETIONENGINE_H
#define COMPLETIONENGINE_H
#include <QListWidget>
#include <QStringList>
#include <QString>

class WidgetTextEdit;
class WidgetTooltip;


struct BibItem
{
    QString key;
    QString title;
    QString author;
};

class CompletionEngine : public QListWidget
{
    Q_OBJECT

public:
    CompletionEngine(WidgetTextEdit * parent);
    ~CompletionEngine();
    void proposeCommand(int left, int top, int lineHeight, QString commandBegin);
    QString acceptedWord();

    void addCustomWordFromSource();
    void parseBibtexFile();
    const QStringList customWords() const { return _customWords; }
public slots:
 //   void setFocus(void);
    void cellSelected(int);
    void setVisible(bool visible);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void loadFile(QString filename);
    QList<BibItem> parseBibtexSource(QString source);

    QString _commandBegin;
    QStringList _customWords;
    WidgetTextEdit * _widgetTextEdit;
    WidgetTooltip * _widgetTooltip;
    QStringList _words;

};

#endif // COMPLETIONENGINE_H
