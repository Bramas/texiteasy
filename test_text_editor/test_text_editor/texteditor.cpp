#include "texteditor.h"
#include "syntaxhighlighter.h"
#include <QFile>
#include <QDebug>
#include <QtScript/QScriptEngine>

TextEditor::TextEditor(QWidget *parent, Tester *tester) :
    QPlainTextEdit(parent),
    _tester(tester)
{

    connect(tester, SIGNAL(textChanged()), this, SLOT(onTextChanged()));


    this->setPlainText("${text}\n${option}\n: if(text == \"cool\") print(\"test\")\n:else print(\"pas cool\")\n"
                       "truuuuc\n:if(option == \"ooo\") print(\"iiii\")");
    this->setPlainText("\\begin{${env}}\n\t<?\nif(env == \"itemize\") print(\"\\\\item \")\n?>${text}\nttt");

    QString insert = _scriptEngine.parse(this->toPlainText(), _tester);
    _tester->findNext();




}
void TextEditor::onTextChanged()
{
    _scriptEngine.evaluate();
}
