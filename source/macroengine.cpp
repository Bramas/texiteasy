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

#include "macroengine.h"
#include "configmanager.h"
#include "filemanager.h"
#include "dialogmacros.h"
#include <QDir>
#include <QFile>
#include <QAction>
#include <QMenu>

MacroEngine MacroEngine::Instance;
const QString MacroEngine::EmptyMacroString =
"<macro %4>\n"
"    <description>%1</description>\n"
"    <trigger>\n"
"    	<keys>%2</keys>\n"
"    	<leftWord>\n"
"    		%3\n"
"    	</leftWord>\n"
"    </trigger>\n"
"    <content><![CDATA[%5]]></content>\n"
"</macro>";

MacroEngine::MacroEngine() :
    QObject(0)
{

    _handler = new MacroXmlHandler();
    _xmlReader.setContentHandler(_handler);
    _xmlReader.setErrorHandler(_handler);
    _xmlReader.setLexicalHandler(_handler);
}

void MacroEngine::init()
{
    loadMacros();
    _dialogMacro = new DialogMacros(0);
}

QList<QAction*> MacroEngine::actions() const
{
    QList<QAction*> list;
    list.reserve(_macros.count());
    foreach(const Macro & macro, _macros)
    {
        list.append(macro.action);
    }
    return list;
}
QList<Macro> MacroEngine::tabMacros() const
{
    QList<Macro> list;
    list.reserve(_tabMacroNames.count());
    foreach(const QString & name, _tabMacroNames)
    {
        Macro macro = _macros.value(name);
        if(!macro.name.isEmpty())
        {
            list.append(macro);
        }
    }
    return list;
}

void MacroEngine::loadMacros()
{
    _macrosPath = ConfigManager::Instance.macrosPath();


    QDir dir(_macrosPath);
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\"+ConfigManager::MacroSuffix+"$"));
    list.replaceInStrings(QRegExp("\\"+ConfigManager::MacroSuffix+"$"), "");

    foreach(const QString & macroName, list)
    {
        loadMacro(macroName);
    }

    list = dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
    foreach(const QString & dirName, list)
    {
        QStringList subList = QDir(_macrosPath+dirName).entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\"+ConfigManager::MacroSuffix+"$"));
        subList.replaceInStrings(QRegExp("\\"+ConfigManager::MacroSuffix+"$"), "");
        foreach(const QString & macroName, subList)
        {
            loadMacro(dirName+"/"+macroName);
        }
    }
}
void MacroEngine::loadMacro(QString name)
{
    QFile file(_macrosPath+name+ConfigManager::MacroSuffix);
    if(!file.open(QFile::ReadOnly))
    {
        qWarning()<<"Unable to open macro file "<<name;
        return;
    }
    _handler->clean();
    QXmlInputSource *source = new QXmlInputSource(&file);
    bool ok = _xmlReader.parse(source);
    delete source;
    if (!ok)
    {
        return;
    }

    Macro macro;
    macro.name = name;
    macro.content = _handler->content();
    macro.keys = _handler->keys();
    macro.leftWord = _handler->leftWord();
    macro.description = _handler->description();
    macro.readOnly = _handler->isReadOnly();


    macro.action = createAction(macro);

    _macros.insert(macro.name, macro);
    if(!macro.keys.compare("Tab", Qt::CaseInsensitive))
    {
        _tabMacroNames.append(macro.name);
    }

}

QAction * MacroEngine::createAction(Macro macro)
{
    QString name = macro.name;
    if(name.contains('/'))
    {
        name = name.split('/').at(1);
    }
    QAction * a = new QAction(name, this);
    a->setShortcut(QKeySequence(macro.keys));
    a->setProperty("macroName", macro.name);
    connect(a, SIGNAL(triggered()), this, SLOT(onMacroTriggered()));
    return a;
}

QMenu * MacroEngine::createMacrosMenu(QMenu * root)
{


    QSettings settings;
    settings.beginGroup("shortcuts");

    QAction * a = root->addAction(trUtf8("Ouvrir l'éditeur de macros"));
    a->setShortcut(QKeySequence(settings.value(trUtf8("Ouvrir l'éditeur de macros")).toString()));
    connect(a, SIGNAL(triggered()), _dialogMacro,SLOT(show()));
    root->addSeparator();

    QMap<QString, QMenu*> macrosSubmenu;
    QMenu * macroMenu;
    foreach(const Macro& macro, orderedMacros())
    {
        QString name = macro.name;
        if(name.contains('/'))
        {
            QStringList l = name.split('/');
            name = l.at(1);
            QString folder = l.at(0);
            macroMenu = macrosSubmenu.value(folder, 0);
            if(!macroMenu)
            {
                macroMenu = root->addMenu(trUtf8(folder.toUtf8().data()));
                macrosSubmenu.insert(folder, macroMenu);
            }
            macroMenu->addAction(createAction(macro));

        }
        else
        {
            root->addAction(createAction(macro));
        }
    }
    _macroMenus.append(root);
    return root;
}

void MacroEngine::onMacroTriggered()
{
    QAction * action = qobject_cast<QAction*>(sender());
    if(action)
    {
        QString name = action->property("macroName").toString();
        Macro macro = _macros.value(name);
        if(!macro.name.isEmpty())
        {
            FileManager::Instance.onMacroTriggered(macro);
        }
    }
}

void MacroEngine::saveMacro(QString name, QString description="", QString keys ="", QString leftWord="", QString content="")
{
    if(name.isEmpty())
    {
        return;
    }
    QFile file(_macrosPath+name+ConfigManager::MacroSuffix);
    if(!file.open(QFile::WriteOnly))
    {
        qWarning()<<"Unable to open macro file "<<name;
        return;
    }
    Macro macro = _macros.value(name);

    QString readOnly("");
    if(!macro.name.isEmpty() && macro.readOnly)
    {
        readOnly = "readOnly=\"1\"";
    }
    file.write(EmptyMacroString
               .arg(description)
               .arg(keys)
               .arg(leftWord)
               .arg(readOnly)
               .arg(content).toUtf8());
    file.close();

    if(macro.name.isEmpty())
    {
        loadMacro(name);
    }
    else
    {
        macro.content = content;
        macro.description = description;
        macro.leftWord = leftWord;
        macro.keys = keys;
        macro.action->setShortcut(QKeySequence(macro.keys));
        _macros.insert(name, macro);
        if(!macro.keys.compare("Tab", Qt::CaseInsensitive))
        {
            if(!_tabMacroNames.contains(name))
            {
                _tabMacroNames.append(name);
            }
        }
        else
        {
            _tabMacroNames.removeOne(name);
        }
    }
    emit changed();
}

bool MacroEngine::rename(QString oldName, QString newName)
{
    Macro macro;
    if(oldName.isEmpty())
    {
        saveMacro(newName,newName);
        return true;
    }
    if(!oldName.compare(newName))
    {
        return false;
    }

    macro = _macros.value(oldName);

    if(macro.name.isEmpty())
    {
        qWarning()<<"macro "<<oldName<<" does not exists.";
        return false;
    }
    QFile file(_macrosPath+macro.name+ConfigManager::MacroSuffix);
    if(!file.exists())
    {
        qWarning()<<"file "<<_macrosPath+macro.name+ConfigManager::MacroSuffix<<" does not exists.";
        return false;
    }
    macro.name = newName;
    //QString oldName = macro.name;
    //macro.name.replace(QRegExp("^([^\\/]+\/){0,1}[^\\/]*$"), "\\1"+newLastName);
    if(file.rename(_macrosPath+macro.name+ConfigManager::MacroSuffix))
    {
        _macros.remove(oldName);
        _macros.insert(macro.name, macro);
        emit changed();
        return true;
    }
    else
    {
        qWarning()<<"Unable to rename macro "<<oldName+ConfigManager::MacroSuffix<<" to "<<macro.name+ConfigManager::MacroSuffix;
    }
    return false;

}
QList<Macro> MacroEngine::orderedMacros()
{
    QList<Macro> list;
    QSettings settings;
    settings.beginGroup("Macros");
    foreach(const QString & macroName, settings.value("macrosOrder").toStringList())
    {
        Macro macro = _macros.value(macroName);
        if(!macro.name.isEmpty())
        {
            list << macro;
        }
    }
    foreach(const Macro & macro, _macros)
    {
        if(!list.contains(macro))
        {
            list << macro;
        }
    }
    return list;
}


bool MacroXmlHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(!qName.compare("macro", Qt::CaseInsensitive))
    {
        _readOnlyAttr = atts.value("readOnly").toInt();
    }else
    if(!qName.compare("keys", Qt::CaseInsensitive) || !qName.compare("description", Qt::CaseInsensitive) || !qName.compare("leftWord", Qt::CaseInsensitive))
    {
        _stringBuffer.clear();
        _readChar = true;
    }
    return true;
}
bool MacroXmlHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    if(!qName.compare("keys", Qt::CaseInsensitive))
    {
        _keys = _stringBuffer.trimmed();
    }
    else if(!qName.compare("leftWord", Qt::CaseInsensitive))
    {
        _leftWord = _stringBuffer.trimmed();
    }
    else if(!qName.compare("description", Qt::CaseInsensitive))
    {
        _description = _stringBuffer.trimmed();
    }
    _readChar = false;
    return true;
}
bool MacroXmlHandler::startCDATA()
{
     _stringBuffer.clear();
     _readChar = true;
    return true;
}
bool MacroXmlHandler::endCDATA()
{
    _content = _stringBuffer;
    _readChar = false;
    return true;
}
