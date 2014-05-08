#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QtScript/QScriptEngine>
#include <QTextCursor>
#include <QVector>
#include <QMap>
#include <QPlainTextEdit>
#include <QMutex>

class WidgetTextEdit;

struct ScriptBlock
{
    QTextCursor cursor;
    int position;
    int length;
    QString insert;
    QString script;
};
struct VarBlock
{
    VarBlock() : active(false) { }
    QTextCursor cursor;
    QTextCursor leftCursor;
    QTextCursor rightCursor;
    QString name;
    bool active;
    int number;
};

class ScriptCursor: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int position READ position WRITE setPosition)

public:
    ScriptCursor(QPlainTextEdit *parent = 0): QObject(parent), _cursor(0) { _plainTextEdit = parent; }

    void setEditor(QPlainTextEdit *parent) { _plainTextEdit = parent; }

    int     position()              { return cursor()->position(); }
    void setPosition(int p)         { cursor()->setPosition(p); }

    QTextCursor *cursor()
    {
        if(!_cursor)
        {
            _cursor = new QTextCursor(_plainTextEdit->textCursor());
        }
        return _cursor;
    }

public Q_SLOTS:
    void             moveStart()    { cursor()->movePosition(QTextCursor::Start); }
    void                moveUp()    { cursor()->movePosition(QTextCursor::Up); }
    void       moveStartOfLine()    { cursor()->movePosition(QTextCursor::StartOfLine); }
    void      moveStartOfBlock()    { cursor()->movePosition(QTextCursor::StartOfBlock); }
    void       moveStartOfWord()    { cursor()->movePosition(QTextCursor::StartOfWord); }
    void     movePreviousBlock()    { cursor()->movePosition(QTextCursor::PreviousBlock); }
    void movePreviousCharacter()    { cursor()->movePosition(QTextCursor::PreviousCharacter); }
    void      movePreviousWord()    { cursor()->movePosition(QTextCursor::PreviousWord); }
    void              moveLeft()    { cursor()->movePosition(QTextCursor::Left); }
    void          moveWordLeft()    { cursor()->movePosition(QTextCursor::WordLeft); }
    void               moveEnd()    { cursor()->movePosition(QTextCursor::End); }
    void              moveDown()    { cursor()->movePosition(QTextCursor::Down); }
    void         moveEndOfLine()    { cursor()->movePosition(QTextCursor::EndOfLine); }
    void         moveEndOfWord()    { cursor()->movePosition(QTextCursor::EndOfWord); }
    void        moveEndOfBlock()    { cursor()->movePosition(QTextCursor::EndOfBlock); }
    void         moveNextBlock()    { cursor()->movePosition(QTextCursor::NextBlock); }
    void     moveNextCharacter()    { cursor()->movePosition(QTextCursor::NextCharacter); }
    void          moveNextWord()    { cursor()->movePosition(QTextCursor::NextWord); }
    void             moveRight()    { cursor()->movePosition(QTextCursor::Right); }
    void         moveWordRight()    { cursor()->movePosition(QTextCursor::WordRight); }
    void                 apply()    { return _plainTextEdit->setTextCursor(*cursor()); }

    void update() {
        if(_cursor)
        {
            delete _cursor;
        }
        cursor();
    }
    void setTextCursor(const QTextCursor & c) {
        if(_cursor)
        {
            delete _cursor;
        }
        _cursor = new QTextCursor(c);
    }

    void insertText(const QString &text)        { cursor()->insertText(text); }

    QString toString() const                    { return QLatin1String("Cursor"); }

protected:
    QTextCursor * _cursor;
    QPlainTextEdit * _plainTextEdit;
};

class ScriptEngine : public QScriptEngine
{
    Q_OBJECT
public:
    explicit ScriptEngine();
    void updateCursors();
    void evaluate();
    void evaluate(QString script) { QScriptEngine::evaluate(script); }
    QString parse(QString text, QPlainTextEdit * editor, const QVector<QString> &varValuesByNumber);
    void appendToCurrentVar(QString s) { _currentVarValue + s; }
    void setCurrentVar(QString var) { _currentVar = var; _currentVarValue = ""; }
    void clear() { _scriptBlocks.clear(); _varTextCursor.clear(); }
    QVector<VarBlock> & varTextCursor() { return _varTextCursor; }
    QMutex * cursorsMutex() { return &_cursorsMutex; }
    void setWidgetTextEdit(WidgetTextEdit * w);
    WidgetTextEdit * widgetTextEdit() { return _widgetTextEdit; }

    QMap<QString, QString> & varValuesByName() { return this->_varValuesByName; }
    QVector<QString> & varValuesByNumber() { return this->_varValuesByNumber; }
    ScriptCursor *getScriptCursor();

private:
    void initVariables(QString text);
signals:

public slots:

private:
    QVector<ScriptBlock> _scriptBlocks;
    QVector<VarBlock> _varTextCursor;
    QMap<QString, QString> _varValuesByName;
    QVector<QString> _varValuesByNumber;
    QString _currentVarValue;
    QString _currentVar;
    QMutex _mutex;
    QMutex _cursorsMutex;
    WidgetTextEdit * _widgetTextEdit;
    QString _script;
    VarBlock _scriptPosition;

};

#endif // SCRIPTENGINE_H
