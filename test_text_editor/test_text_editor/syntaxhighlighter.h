#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument * parent  = 0);
    
signals:
    
public slots:

protected:
    void highlightBlock(const QString &text);
    
};

#endif // SYNTAXHIGHLIGHTER_H
