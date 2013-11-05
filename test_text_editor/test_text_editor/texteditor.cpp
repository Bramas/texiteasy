#include "texteditor.h"
#include "syntaxhighlighter.h"
#include <QFile>
#include <QDebug>

TextEditor::TextEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    (void) new SyntaxHighlighter(this->document());
    QFile f("/Users/quentinbramas/Projects/texiteasy/texiteasy-repository/test_text_editor/lorem.txt");
    if(f.open(QFile::ReadOnly | QFile::Text))
    {
        this->setPlainText(f.readAll());
    }
    else
    {
        qDebug()<<f.errorString();
    }
}
