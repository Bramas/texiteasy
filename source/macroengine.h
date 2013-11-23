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

#ifndef MACROENGINE_H
#define MACROENGINE_H

#include <QObject>
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QDebug>
#include <QMap>
#include <QList>

class QMenu;
class QAction;
class DialogMacros;

/**
 * @brief The MacroXmlHandler class read a macro file and store useful information (description, keys ...)
 * @author Quentin BRAMAS
 */

class MacroXmlHandler : public QXmlDefaultHandler
{
public:
    MacroXmlHandler() : _readChar(false) { }
    QString keys() { return _keys; }
    QString leftWord() { return _leftWord; }
    bool isReadOnly() { return _readOnlyAttr; }
    QString content() { return _content; }
    QString description() { return _description; }
    void clean() { _keys.clear(); _leftWord.clear(); _content.clear(); _description.clear(); _readOnlyAttr=false; }

    bool fatalError(const QXmlParseException & exception)
    {
        qWarning() << "Fatal error on line" << exception.lineNumber()
                   << ", column" << exception.columnNumber() << ":"
                   << exception.message();

        return false;
    }
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    virtual bool startCDATA();
    virtual bool endCDATA();
    bool characters(const QString & ch) { if(_readChar) _stringBuffer+=ch; return true; }


protected:
    QString _stringBuffer;
    QString _keys;
    QString _leftWord;
    QString _description;
    QString _content;
    bool _readOnlyAttr;
    bool _readChar;
};

struct Macro
{
    QString name;
    QString keys;
    QString content;
    QString leftWord;
    QString description;
    QAction * action;
    bool readOnly;
    Macro() : readOnly(false) {}

    bool operator==(const Macro & other)
    {
        return this->name == other.name;
    }
};

/**
 * @brief The MacroEngine class loads the macros directory, and store macros.
 * @author Quentin BRAMAS
 *
 * The MacroEngine class create actions for macros and listen to action trigger. When an action is triggered it informes the filemanager.
 * The class also create menus with macros.
 */

class MacroEngine : public QObject
{
    Q_OBJECT
public:
    static MacroEngine Instance;
    static const QString EmptyMacroString;

    const QMap<QString, Macro> & macros() const { return _macros; }

    QList<Macro> orderedMacros();
    QMenu * createMacrosMenu(QMenu *root);
    QAction * createAction(Macro macro);
    void init();
    void loadMacros();
    void loadMacro(QString name);
    void saveMacro(QString name, QString description, QString keys, QString leftWord, QString content);
    bool deleteMacro(QString name);
    QList<QAction*> actions() const;
    QList<Macro> tabMacros() const;

    bool renameFolder(QString oldName, QString newName);
    bool rename(QString oldName, QString newName);
signals:
    void changed();

public slots:
    void onMacroTriggered();

private:
    explicit MacroEngine();

    MacroXmlHandler * _handler;
    QXmlSimpleReader _xmlReader;
    QString _macrosPath;
    DialogMacros * _dialogMacro;

    QMap<QString, Macro> _macros;
    QStringList _tabMacroNames;
    QList<QMenu*> _macroMenus;
    //QMap<QString, QAction *> _macroActions;

};

#endif // MACROENGINE_H
