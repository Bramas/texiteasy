#include "scriptengine.h"
#include <QDebug>


QString scriptOutput;
QScriptValue scriptPrint(QScriptContext *context, QScriptEngine *engine)
{
   QScriptValue a = context->argument(0);
   scriptOutput += a.toString();
   return QScriptValue();
}


ScriptEngine::ScriptEngine() :
    QScriptEngine()
{
    QScriptValue scriptPrintValue = this->newFunction(scriptPrint);
    this->globalObject().setProperty("write", scriptPrintValue);
}


QString ScriptEngine::parse(QString text, QPlainTextEdit *editor)
{
    QTextCursor cursor = editor->textCursor();
    cursor.removeSelectedText();

    QString scriptBuffer;
    QString insert;
    QChar ch, nextCh;
    int insertIndex = 0;
    int index = 0;
    int state = 0;
    while(index < text.size() - 1)
    {
        ch = text.at(index);
        nextCh = text.at(index+1);
        if(state == 0)
        {
            if(ch == '<' && nextCh == '?')
            {
                index+= 2;
                state = 1;
                continue;
            }
            ++insertIndex;
            insert += ch;
        }
        else
        {
            if(ch == '?' && nextCh == '>')
            {
                index+= 2;
                state = 0;
                ScriptBlock sb;
                sb.length = 0;
                sb.script = scriptBuffer;
                sb.position = insertIndex;
                _scriptBlocks.append(sb);
                scriptBuffer.clear();
                continue;
            }
            scriptBuffer += ch;
        }
        ++index;
    }
    insert += text.right(1);

    int position = cursor.position();
    cursor.insertText(insert);
    for(int i = 0; i < _scriptBlocks.count(); ++i)
    {
        _scriptBlocks[i].position += position - 1;
        _scriptBlocks[i].cursor = editor->textCursor();
        _scriptBlocks[i].cursor.setPosition(_scriptBlocks.at(i).position);
    }

    int pIdx = -1;
    QStringList varNames;
    QRegExp p("\\$\\{([0-9]:){0,1}([^\\}]*)\\}");
    while(-1 != (pIdx = insert.indexOf(p, pIdx + 1)))
    {
        VarBlock vb;
        vb.name = p.capturedTexts().at(2);
        if(varNames.contains(vb.name))
        {
            continue;
        }
        varNames << vb.name;
        vb.cursor = editor->textCursor();
        vb.leftCursor = editor->textCursor();
        vb.rightCursor = editor->textCursor();
        vb.cursor.setPosition(position + pIdx);
        vb.leftCursor.setPosition(position + pIdx - 1);
        vb.rightCursor.setPosition(position + pIdx + p.capturedTexts().at(0).length() + 1);
        vb.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, p.capturedTexts().at(0).length());
        _varTextCursor.append(vb);
        //qDebug()<<position + pIdx;
    }

    foreach(VarBlock vb, _varTextCursor)
    {
        //qDebug()<<vb.name<<" = "<<vb.cursor.selectedText();
    }

    return insert;
}

void ScriptEngine::evaluate()
{
    if(!_mutex.tryLock())
    {
        return;
    }

    //qDebug()<<"_scriptBlocks[i].position";
    foreach(ScriptBlock sb, _scriptBlocks)
    {
        ////qDebug()<<sb.cursor.selectionStart();
    }

    updateCursors();


    foreach(VarBlock vb, _varTextCursor)
    {
        QString v = vb.cursor.selectedText();
        ////qDebug()<<vb.name<<" = "<<v;
        this->evaluate("var "+vb.name+" = \""+v+"\"");
    }



    for(int i = 0; i < _scriptBlocks.count(); ++i)
    {
       ScriptBlock sb = _scriptBlocks.at(i);
       scriptOutput.clear();
       this->evaluate(sb.script);

       if(sb.length)
       {
           sb.cursor.setPosition(sb.cursor.selectionStart());
           sb.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, sb.length);
           sb.cursor.removeSelectedText();
       }
       else
       {
           sb.cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
       }
       sb.length = scriptOutput.size();
       if(sb.length)
       {
            sb.cursor.insertText(scriptOutput);
            sb.cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, sb.length);
       }
       else
       {
           sb.cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
       }
       _scriptBlocks[i] = sb;

    }

    updateCursors();

    //qDebug()<<"-------end";

    _mutex.unlock();
}

void ScriptEngine::updateCursors()
{
    //qDebug()<<"_varTextCursor[i].position";
    for(int i = 0; i < _varTextCursor.count(); ++i)
    {
        VarBlock vb = _varTextCursor.at(i);
        //qDebug()<<vb.cursor.selectionStart()<<" "<<vb.leftCursor.position()<<","<<vb.rightCursor.position();
        if(vb.cursor.selectedText().isEmpty())
        {
            // in this case we lost the main cursor
            // if the two others are really close, we choose the left cursor
            // else we find it based on the position of the left cursor
            //qDebug()<<"correction 1";
            vb.cursor = vb.leftCursor;
            if(vb.rightCursor.position() - vb.leftCursor.position() >= 2)
            {
                //qDebug()<<"correction 1.2";
                vb.cursor = vb.leftCursor;
                vb.cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
                vb.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, vb.rightCursor.position() - vb.leftCursor.position() - 2);
            }
        }
        else
        if(vb.cursor.selectedText().length() != vb.rightCursor.position() - vb.leftCursor.position() - 2)
        {
            //qDebug()<<"correction 2";
            //in this case we lost the two other cursors
            vb.leftCursor = vb.cursor;
            vb.rightCursor = vb.cursor;
            vb.leftCursor.setPosition(vb.cursor.selectionStart() - 1);
            vb.rightCursor.setPosition(vb.cursor.selectionEnd() + 1);
        }
        _varTextCursor[i] = vb;
        //qDebug()<<vb.cursor.selectionStart()<<" "<<vb.leftCursor.position()<<","<<vb.rightCursor.position();
    }
}
