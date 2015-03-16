#ifndef TEXTACTION_H
#define TEXTACTION_H

#include <QObject>
#include <QTextCursor>
class WidgetFile;

class TextAction : public QObject
{
    Q_OBJECT
public:
    explicit TextAction();
    ~TextAction();

    bool execute(QTextCursor clickCursor, WidgetFile * widgetFile);
    QTextCursor match(QTextCursor clickCursor, WidgetFile * widgetFile);
    int linkTextStart() { return _linkTextStart; }
    int linkTextEnd() {return _linkTextEnd; }

signals:

public slots:

private:
    int _linkTextStart;
    int _linkTextEnd;
};

#endif // TEXTACTION_H
