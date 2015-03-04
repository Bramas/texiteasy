#include "textaction.h"
#include "widgetfile.h"
#include "widgettextedit.h"
#include "completionengine.h"

#include <QDebug>
#include <QTextCursor>


TextAction::TextAction()
{

}

TextAction::~TextAction()
{

}

bool TextAction::execute(QTextCursor clickCursor, WidgetFile *widgetFile)
{
    int left, right;
    left = right = clickCursor.position();
    while(widgetFile->widgetTextEdit()->nextChar(clickCursor) != '\\')
    {
        --left;
        clickCursor.setPosition(left, QTextCursor::MoveAnchor);
    }
    clickCursor.setPosition(right, QTextCursor::MoveAnchor);
    while(QString(widgetFile->widgetTextEdit()->nextChar(clickCursor)).contains(QRegExp("[a-zA-Z0-9*]")))
    {
        ++right;
        clickCursor.setPosition(right, QTextCursor::MoveAnchor);
    }
    clickCursor.setPosition(left, QTextCursor::KeepAnchor);
    QString command = clickCursor.selectedText();
    foreach(const QString &word, widgetFile->widgetTextEdit()->completionEngine()->customWords())
    {
        if(word == command)
        {
            QString newCommand = "\\\\newcommand\\{\\"+command+"\\}";
            if(widgetFile->widgetTextEdit2()->find(QRegExp(newCommand), QTextDocument::FindBackward))
            {
                widgetFile->splitEditor(true);
                widgetFile->widgetTextEdit2()->ensureCursorVisible();
                return true;
            }
        }
    }
    return false;
}

