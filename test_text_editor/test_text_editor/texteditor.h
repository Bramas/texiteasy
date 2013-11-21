#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QPlainTextEdit>
#include <QDebug>
#include <QtScript/QScriptEngine>
#include <QVector>
#include <QMap>

struct ScriptBlock
{
    QTextCursor cursor;
    int position;
    QString script;
};

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
        if(e->text().contains(QRegExp("[a-zA-Z0-9_-=+*]")))
        {
            varValue += e->text();
            var = "var "+currentVarName+" = \""+varValue+"\"";
        }
        else
        if(e->key() == Qt::Key_Backspace)
        {
            varValue = varValue.left(varValue.size() - 1);
            var = "var "+currentVarName+" = \""+varValue+"\"";
        }else
        if(e->key() == Qt::Key_Tab)
        {
            declarations.append(var);
            stop=true;
            findNext();
            return;
        }
        else
        {
            if(!stop)
            {
                declarations.append(var);
                stop = true;
            }
        }
        QPlainTextEdit::keyPressEvent(e);
        if(!stop)
        {
            emit keyPressed();
        }
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
    void setVar(QStringList declaration, QString var)
    {
        _var = var+"\n";
        foreach(QString var, declaration)
        {
            _var += var+"\n";
        }
        onTextChanged();
    }
    
signals:
    
public slots:
    void onTextChanged();

private:
    QTextCursor _cursor;
    QString _var;
    int _pos;
    Tester * _tester;
    QVector<ScriptBlock> _scriptBlocks;
    
};

#endif // TEXTEDITOR_H
