#ifndef MACROENGINE_H
#define MACROENGINE_H

#include <QObject>
#include <QXmlDefaultHandler>
#include <QXmlSimpleReader>
#include <QDebug>
#include <QMap>
#include <QList>

class QAction;

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
    bool readOnly = false;
};

class MacroEngine : public QObject
{
    Q_OBJECT
public:
    static MacroEngine Instance;
    static const QString EmptyMacroString;

    const QMap<QString, Macro> & macros() const { return _macros; }
    void loadMacros();
    void loadMacro(QString name);
    void saveMacro(QString name, QString description, QString keys, QString leftWord, QString content);
    QList<QAction*> actions() const;
    QList<Macro> tabMacros() const;
signals:

public slots:
    void onMacroTriggered();

private:
    explicit MacroEngine();

    MacroXmlHandler * _handler;
    QXmlSimpleReader _xmlReader;
    QString _macrosPath;

    QMap<QString, Macro> _macros;
    QStringList _tabMacroNames;
    //QMap<QString, QAction *> _macroActions;

};

#endif // MACROENGINE_H
