#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QtScript/QScriptEngine>
#include <QTextCursor>
#include <QVector>
#include <QMap>
#include <QPlainTextEdit>
#include <QMutex>

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
    QTextCursor cursor;
    QTextCursor leftCursor;
    QTextCursor rightCursor;
    QString name;
};

class ScriptEngine : public QScriptEngine
{
    Q_OBJECT
public:
    explicit ScriptEngine();
    void updateCursors();
    void evaluate();
    void evaluate(QString script) { QScriptEngine::evaluate(script); }
    QString parse(QString text, QPlainTextEdit * editor);
    void appendToCurrentVar(QString s) { _currentVarValue + s; }
    void setCurrentVar(QString var) { _currentVar = var; _currentVarValue = ""; }
    void clear() { _scriptBlocks.clear(); _varTextCursor.clear(); }
    QMutex * cursorsMutex() { return &_cursorsMutex; }
signals:

public slots:

private:
    QVector<ScriptBlock> _scriptBlocks;
    QVector<VarBlock> _varTextCursor;
    QString _currentVarValue;
    QString _currentVar;
    QMutex _mutex;
    QMutex _cursorsMutex;

};

#endif // SCRIPTENGINE_H
