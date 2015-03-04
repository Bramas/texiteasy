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

signals:

public slots:
};

#endif // TEXTACTION_H
