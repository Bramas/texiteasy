#ifndef TEXTACTION_H
#define TEXTACTION_H

#include <QObject>
#include <QTextCursor>
#include <QVector>

class WidgetFile;

class AbstractTextAction;

class TextActions
{
public:
    static bool execute(QTextCursor clickCursor, WidgetFile * widgetFile);
    static QTextCursor match(QTextCursor clickCursor, WidgetFile * widgetFile);

private:
    static QVector<AbstractTextAction*> _textActions;
};

class AbstractTextAction : public QObject
{
    Q_OBJECT
public:
    virtual bool execute(QTextCursor clickCursor, WidgetFile * widgetFile) = 0;
    virtual QTextCursor match(QTextCursor clickCursor, WidgetFile * widgetFile) = 0;


};


class CustomCommandTextAction : public AbstractTextAction
{
    Q_OBJECT
public:
    explicit CustomCommandTextAction();

    bool execute(QTextCursor clickCursor, WidgetFile * widgetFile);
    QTextCursor match(QTextCursor clickCursor, WidgetFile * widgetFile);

};


class RefLinkTextAction : public AbstractTextAction
{
    Q_OBJECT
public:
    explicit RefLinkTextAction();

    bool execute(QTextCursor clickCursor, WidgetFile * widgetFile);
    QTextCursor match(QTextCursor clickCursor, WidgetFile * widgetFile);

};

class CiteLinkTextAction : public AbstractTextAction
{
    Q_OBJECT
public:
    explicit CiteLinkTextAction();

    bool execute(QTextCursor clickCursor, WidgetFile * widgetFile);
    QTextCursor match(QTextCursor clickCursor, WidgetFile * widgetFile);

};

#endif // TEXTACTION_H
