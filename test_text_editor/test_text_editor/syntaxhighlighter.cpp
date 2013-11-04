#include "syntaxhighlighter.h"
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat format[5];
    format[0].setForeground(QBrush(QColor(100,0,0)));
    format[1].setForeground(QBrush(QColor(0,100,0)));
    format[2].setForeground(QBrush(QColor(0,0,100)));
    format[3].setForeground(QBrush(QColor(100,100,0)));
    format[4].setForeground(QBrush(QColor(100,0,100)));
    int i = 0;
    while(i < text.length())
    {
        setFormat(i, 1, format[i%5]);
        ++i;
    }
}
