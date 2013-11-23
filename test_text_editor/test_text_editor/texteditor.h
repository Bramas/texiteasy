#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>
#include <QDebug>
#include <QtScript/QScriptEngine>
#include <QVector>
#include <QMap>
#include "scriptengine.h"


class Tester : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Tester(QWidget *parent = 0) : QPlainTextEdit(parent), stop(false) { }
    QString var;
    QStringList declarations;

    void findNext()
    {
        QTextCursor cur = this->document()->find(QRegExp("\\$\\{[^\\}]*\\}"));
        if(cur.isNull())
        {
            return;
        }
        this->setTextCursor(cur);
        currentVarName = cur.selectedText();
        currentVarName.replace(QRegExp("\\$\\{([^\\}]*)\\}"), "\\1");
        qDebug()<<"varName "<<currentVarName;
        varValue = "";
        stop = false;
    }
protected:
    void keyPressEvent(QKeyEvent *e)
    {
        if(e->key() == Qt::Key_Tab)
        {
            findNext();
            return;
        }
        QPlainTextEdit::keyPressEvent(e);
        keyPressed();
    }

signals:
    void keyPressed();
private:
    bool stop;
    QString currentVarName;
    QString varValue;
};

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent, Tester * tester);
    signals:
    
public slots:
    void onTextChanged();

private:
    QTextCursor _cursor;
    int _pos;
    Tester * _tester;
    ScriptEngine _scriptEngine;
    
};

#endif // TEXTEDITOR_H
