#include "macroengine.h"
#include "configmanager.h"
#include "filemanager.h"
#include <QDir>
#include <QFile>
#include <QAction>

MacroEngine MacroEngine::Instance;
const QString MacroEngine::EmptyMacroString =
"<macro>\n"
"    <description>%1</description>\n"
"    <trigger>\n"
"    	<keys>%2</keys>\n"
"    	<leftWord>\n"
"    		%3\n"
"    	</leftWord>\n"
"    </trigger>\n"
"    <content><![CDATA[%4]]></content>\n"
"</macro>";

MacroEngine::MacroEngine() :
    QObject(0)
{

    _handler = new MacroXmlHandler();
    _xmlReader.setContentHandler(_handler);
    _xmlReader.setErrorHandler(_handler);
    _xmlReader.setLexicalHandler(_handler);
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

    QAction * a = new QAction(name, this);
    a->setShortcut(QKeySequence(macro.keys));
    a->setProperty("macroName", name);
    macro.action = a;
    connect(a, SIGNAL(triggered()), this, SLOT(onMacroTriggered()));

    _macros.insert(name, macro);
    if(!macro.keys.compare("Tab", Qt::CaseInsensitive))
    {
        _tabMacroNames.append(name);
    }

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

void MacroEngine::saveMacro(QString name, QString description, QString keys, QString leftWord, QString content)
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
    file.write(EmptyMacroString
               .arg(description)
               .arg(keys)
               .arg(leftWord)
               .arg(content).toUtf8());
    Macro macro = _macros.value(name);
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
