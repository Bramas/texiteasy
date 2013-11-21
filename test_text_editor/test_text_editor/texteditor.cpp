#include "texteditor.h"
#include "syntaxhighlighter.h"
#include <QFile>
#include <QDebug>
#include <QtScript/QScriptEngine>

TextEditor::TextEditor(QWidget *parent, Tester *tester) :
    QPlainTextEdit(parent),
    _tester(tester)
{

    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));


    this->setPlainText("${text}\n${option}\n: if(text == \"cool\") print(\"test\")\n:else print(\"pas cool\")\n"
                       "truuuuc\n:if(option == \"ooo\") print(\"iiii\")");
    this->setPlainText("\\begin{${env}}\n\t\n:if(env == \"itemize\") print(\"\\\\item \")\n${text}\n");


    int pos = 0;
    QStringList lines = this->toPlainText().split('\n');
    QString scriptBuffer;
    QString insert;
    foreach(const QString & line, lines)
    {
        if(!line.size() || line.at(0) != ':')
        {
            if(scriptBuffer.isEmpty())
            {
                if(pos)
                {
                    insert += "\n";
                    ++pos;
                }
                insert += line;
                pos += line.length();
                continue;
            }

            insert += " "+line;
            ScriptBlock sb;
            sb.script = scriptBuffer;
            sb.position = pos;
            _scriptBlocks.append(sb);
            pos += line.length() + 1;
            scriptBuffer.clear();

            continue;
        }
        if(line.at(0) == '#')
        {
            continue;
        }

        scriptBuffer += line.right(line.size() - 1)+"\n";
    }
    if(!scriptBuffer.isEmpty())
    {
        ScriptBlock sb;
        sb.script = scriptBuffer;
        sb.position = pos;
        _scriptBlocks.append(sb);
    }
    _tester->setPlainText(insert);
    _tester->findNext();

    for(int i = 0; i < _scriptBlocks.count(); ++i)
    {
        ScriptBlock sb = _scriptBlocks.at(i);
        sb.cursor = _tester->textCursor();
        sb.cursor.setPosition(sb.position);
        _scriptBlocks[i] = sb;
        qDebug()<<"cur pos : "<<pos<<" "<<sb.cursor.position();
    }


}
QString scriptOutput;
QScriptValue scriptPrint(QScriptContext *context, QScriptEngine *engine)
{
   QScriptValue a = context->argument(0);
   scriptOutput += a.toString();
   return QScriptValue();
}
void TextEditor::onTextChanged()
{
     QScriptEngine myEngine;
     QScriptValue scriptPrintValue = myEngine.newFunction(scriptPrint);
    myEngine.globalObject().setProperty("print", scriptPrintValue);

    myEngine.evaluate(_var);
    qDebug()<<_var;

    foreach(ScriptBlock sb, _scriptBlocks)
    {
        qDebug()<<"cursor pos :"<<sb.cursor.position();
    }


    for(int i = 0; i < _scriptBlocks.count(); ++i)
    {
        ScriptBlock sb = _scriptBlocks.at(i);
        qDebug()<<"delete : "<<sb.cursor.selectedText();
        sb.cursor.removeSelectedText();
        scriptOutput.clear();
        myEngine.evaluate(sb.script);
        if(!scriptOutput.isEmpty())
        {
            sb.cursor.insertText(scriptOutput);
            sb.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, scriptOutput.size());
            _scriptBlocks[i] = sb;
        }
    }



     //QScriptValue scriptButton = myEngine.newQObject(&s);



}
