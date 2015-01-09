#include "scriptengine.h"
#include <QDebug>
#include "widgettextedit.h"
#include "widgetfile.h"
#include "mainwindow.h"
#include <QElapsedTimer>

#define DISP_DEBUG(a)

QString scriptOutput;
QTextCursor scriptCursor;


QScriptValue scriptPrint(QScriptContext *context, QScriptEngine *engine_in)
{
   ScriptEngine * engine = dynamic_cast<ScriptEngine*>(engine_in);
   QString text = context->argument(0).toString();

   DISP_DEBUG(qDebug()<<"insert : "<<text);


   QTextCursor currentCursor(*engine->getScriptCursor()->cursor());
   int position = currentCursor.position();

   text.replace(QRegExp("\\$\\{([0-9]:){0,1}([^\\}]*)\\}"), "\\verb#{{\\1\\2}}#");
   currentCursor.insertText(text); 

   int pIdx = -1;
   QStringList varNames;
   QRegExp p("\\\\verb\\#\\{\\{([0-9]:){0,1}([^\\}]*)\\}\\}\\#");


   while(-1 != (pIdx = text.indexOf(p, pIdx + 1)))
   {
       VarBlock vb;
       vb.name = p.capturedTexts().at(2);

       if(varNames.contains(vb.name))
       {
           //continue;
       }

       varNames << vb.name;
       if(p.capturedTexts().at(1).size())
       {
            vb.number = p.capturedTexts().at(1).left(p.capturedTexts().at(1).size() - 1).toInt();
       }
       else
       {
           vb.number = 0;
       }
       vb.cursor = currentCursor;
       vb.leftCursor = currentCursor;
       vb.rightCursor = currentCursor;
       vb.cursor.setPosition(position + pIdx);
       vb.leftCursor.setPosition(position + pIdx - 1);
       vb.rightCursor.setPosition(position + pIdx + p.capturedTexts().at(0).length() + 1);
       vb.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, p.capturedTexts().at(0).length());
       engine->varTextCursor().append(vb);
       DISP_DEBUG(qDebug()<<position + pIdx);
   }



   return QScriptValue();
}


QScriptValue scriptUseEditor(QScriptContext *context, QScriptEngine *engine_in)
{
   WidgetTextEdit* editor = dynamic_cast<WidgetTextEdit*>(context->argument(0).toQObject());
   ScriptEngine * engine = dynamic_cast<ScriptEngine*>(engine_in);
   engine->getScriptCursor()->setEditor(editor);
   engine->getScriptCursor()->setTextCursor(editor->textCursor());
   return QScriptValue();
}
QScriptValue scriptDebug(QScriptContext *context, QScriptEngine * /*engine*/)
{
   QScriptValue a = context->argument(0);
   qDebug() << a.toString();
   return QScriptValue();
}
QScriptValue scriptStopEvaluation(QScriptContext * /*context*/, QScriptEngine * /*engine*/)
{
   return QScriptValue();
}




ScriptEngine::ScriptEngine() :
    QScriptEngine()
{
    QScriptValue scriptPrintValue = this->newFunction(scriptPrint);
    this->globalObject().setProperty("write", scriptPrintValue);
    QScriptValue scriptDebugValue = this->newFunction(scriptDebug);
    this->globalObject().setProperty("debug", scriptDebugValue);
    QScriptValue scriptUseEditorValue = this->newFunction(scriptUseEditor);
    this->globalObject().setProperty("useEditor", scriptUseEditorValue);

}


QString sanitize(QString text)
{
    text.replace(QRegExp("(\\\\|')"), "\\\\1");
    text.replace('\n', "\\n");
    return text;
}

void ScriptEngine::initVariables(QString text)
{
    int pIdx = -1;
    QRegExp p("\\$\\{([0-9]:){0,1}([^\\}]*)\\}");
    while(-1 != (pIdx = text.indexOf(p, pIdx + 1)))
    {
        this->evaluate("var "+p.capturedTexts().at(2)+" = ''");
    }
}


ScriptCursor * ScriptEngine::getScriptCursor()
{
    return dynamic_cast<ScriptCursor *>(this->globalObject().property("cursor").toQObject());
}

QString ScriptEngine::parse(QString text, QPlainTextEdit *editor, const QVector<QString> &varValuesByNumber)
{
    if(!_mutex.tryLock())
    {
        return "";
    }

    _cursorsMutex.lock();

    _varValuesByName.clear();
    _varValuesByNumber = varValuesByNumber;

    this->varTextCursor().clear();
    QTextCursor cursor = editor->textCursor();
    cursor.removeSelectedText();
    getScriptCursor()->setTextCursor(cursor);

    _scriptPosition.leftCursor = cursor;
    _scriptPosition.rightCursor = cursor;
    _scriptPosition.leftCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
    _scriptPosition.rightCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);

    QString scriptBuffer;
    QString writeBuffer;
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
                if(!writeBuffer.isEmpty())
                {
                    scriptBuffer += "\nwrite('"+sanitize(writeBuffer)+"');\n";
                }
                writeBuffer = "";
                index+= 2;
                state = 1;
                continue;
            }
            ++insertIndex;
            writeBuffer += ch;
        }
        else
        {
            if(ch == '?' && nextCh == '>')
            {
                index+= 2;
                state = 0;
                continue;
            }
            scriptBuffer += ch;
        }
        ++index;
    }
    if(state == 0 && text.size() && (text.size() < 2 || !(text.at(text.size() - 2) == '?' && text.at(text.size() - 1) == '>')))
    {
        writeBuffer += text.at(text.size() - 1);
    }
    if(!writeBuffer.isEmpty())
    {
        scriptBuffer += "\nwrite('"+sanitize(writeBuffer)+"');\n";
    }

    _script = scriptBuffer;
    DISP_DEBUG(qDebug()<<"scriptBuffer:");
    DISP_DEBUG(qDebug()<<scriptBuffer);
    initVariables(_script);
    this->evaluate(_script);
    QScriptValue exc;
    if(!(exc = this->uncaughtException()).toString().isEmpty())
    {
        qDebug()<<"Uncaught Exception : "<<exc.toString();
    }

    QTextCursor c = _scriptPosition.leftCursor;
    c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
    c.setPosition(_scriptPosition.rightCursor.position(), QTextCursor::KeepAnchor);
    DISP_DEBUG(qDebug()<<"selectedText:");
    DISP_DEBUG(qDebug() << c.selectedText());

    foreach(VarBlock vb, varTextCursor())
    {
        DISP_DEBUG(qDebug()<<vb.number<<":"<<vb.name<<" = "<<vb.cursor.selectedText());
        if(_varValuesByNumber.count() > vb.number && vb.number > 0 && !_varValuesByNumber.value(vb.number).isEmpty())
        {
            vb.cursor.removeSelectedText();
            vb.cursor.insertText(_varValuesByNumber.value(vb.number));
        }
        else
        {
            QString val = vb.cursor.selectedText();
            vb.cursor.removeSelectedText();
            val.replace(QRegExp("\\\\verb\\#\\{\\{([0-9]:){0,1}([^\\}]*)\\}\\}\\#"), "\\verb#{{\\2}}#");
            vb.cursor.insertText(val);
        }
    }
    DISP_DEBUG(qDebug()<<"selectedText:");
    DISP_DEBUG(qDebug() << c.selectedText());
    _mutex.unlock();
    _cursorsMutex.unlock();
    return "";

}

void ScriptEngine::evaluate()
{
    if(!_mutex.tryLock())
    {
        return;
    }
    _cursorsMutex.lock();
    DISP_DEBUG(qDebug()<<"_scriptBlocks[i].position");
    foreach(ScriptBlock sb, _scriptBlocks)
    {
        DISP_DEBUG(qDebug()<<sb.cursor.selectionStart());
    }

    QTextCursor currentCursor = _widgetTextEdit->textCursor();
    currentCursor.joinPreviousEditBlock();
    updateCursors();

    currentCursor = _widgetTextEdit->textCursor();

    _varValuesByName.clear();
    QStringList activeCursors;
    for(int idx = 0; idx < _varTextCursor.count(); ++idx)
    {
        VarBlock vb = _varTextCursor.at(idx);
        QString v = vb.cursor.selectedText();
        DISP_DEBUG(qDebug()<<vb.name<<" = "<<v);
        if(!v.isEmpty())
        {
            this->evaluate("var "+vb.name+" = \""+v+"\"");
            _varValuesByName[vb.name] = v;
        }
        if(currentCursor.position() == vb.rightCursor.position() - 1)
        {
            activeCursors << vb.name;
            DISP_DEBUG(qDebug()<<"ACTIVE : "<<vb.name);
        }
    }
    //int previousCursorPosition = _widgetTextEdit->textCursor().position();
    QTextCursor c = _scriptPosition.leftCursor;
    c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
    c.setPosition(_scriptPosition.rightCursor.position()-1, QTextCursor::KeepAnchor);
    DISP_DEBUG(qDebug()<<"removeText:");
    DISP_DEBUG(qDebug() << c.selectedText());
    c.removeSelectedText();
    _widgetTextEdit->setTextCursor(c);

    _varTextCursor.clear();

    getScriptCursor()->setTextCursor(widgetTextEdit()->textCursor());

    this->evaluate(_script);

    QScriptValue exc;
    if(!(exc = this->uncaughtException()).toString().isEmpty())
    {
        qDebug()<<"Uncaught Exception : "<<exc.toString();
    }

    c = _scriptPosition.leftCursor;
    c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
    c.setPosition(_scriptPosition.rightCursor.position(), QTextCursor::KeepAnchor);
    DISP_DEBUG(qDebug()<<"selectedText:");
    DISP_DEBUG(qDebug() << c.selectedText());

    _widgetTextEdit->clearTextCursors();
    bool firstCursor = true;
    foreach(VarBlock vb, varTextCursor())
    {
        DISP_DEBUG(qDebug()<<vb.number<<":"<<vb.name<<" = "<<vb.cursor.selectedText());
        if(_varValuesByName.contains(vb.name))
        {
            vb.cursor.removeSelectedText();
            QString val = _varValuesByName.value(vb.name);
            vb.cursor.insertText(val);
        }
        else
        {
            QString val = vb.cursor.selectedText();
            vb.cursor.removeSelectedText();
            val.replace(QRegExp("\\\\verb\\#\\{\\{([0-9]:){0,1}([^\\}]*)\\}\\}\\#"), "\\verb#{{\\2}}#");
            vb.cursor.insertText(val);
        }
        if(activeCursors.contains(vb.name))
        {
            QTextCursor previousCursor = vb.rightCursor;
            previousCursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
            if(firstCursor)
            {
                _widgetTextEdit->setTextCursor(previousCursor);
                firstCursor = false;
            }
            else
            {
                _widgetTextEdit->addTextCursor(previousCursor);
            }
        }
    }
    currentCursor = _widgetTextEdit->textCursor();
    currentCursor.endEditBlock();
    _widgetTextEdit->onCursorPositionChange();
    DISP_DEBUG(qDebug()<<"selectedText:");
    DISP_DEBUG(qDebug() << c.selectedText());
    _cursorsMutex.unlock();
    _mutex.unlock();
    return;

}

void ScriptEngine::updateCursors()
{
    DISP_DEBUG(qDebug()<<"_varTextCursor[i].position");
    for(int i = 0; i < _varTextCursor.count(); ++i)
    {
        VarBlock vb = _varTextCursor.at(i);
        DISP_DEBUG(qDebug()<<vb.cursor.selectionStart()<<" "<<vb.leftCursor.position()<<","<<vb.rightCursor.position());
        if(vb.cursor.selectedText().isEmpty())
        {
            // in this case we lost the main cursor
            // if the two others are really close, we choose the left cursor
            // else we find it based on the position of the left cursor
            DISP_DEBUG(qDebug()<<"correction 1");
            vb.cursor = vb.leftCursor;
            if(vb.rightCursor.position() - vb.leftCursor.position() >= 2)
            {
                DISP_DEBUG(qDebug()<<"correction 1.2");
                vb.cursor = vb.leftCursor;
                vb.cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
                vb.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, vb.rightCursor.position() - vb.leftCursor.position() - 2);
            }
        }
        else
        if(vb.cursor.selectedText().length() != vb.rightCursor.position() - vb.leftCursor.position() - 2)
        {
            DISP_DEBUG(qDebug()<<"correction 2");
            //in this case we lost the two other cursors
            vb.leftCursor = vb.cursor;
            vb.rightCursor = vb.cursor;
            vb.leftCursor.setPosition(vb.cursor.selectionStart() - 1);
            vb.rightCursor.setPosition(vb.cursor.selectionEnd() + 1);
        }
        _varTextCursor[i] = vb;
        DISP_DEBUG(qDebug()<<vb.cursor.selectionStart()<<" "<<vb.leftCursor.position()<<","<<vb.rightCursor.position());
    }
}

void ScriptEngine::setWidgetTextEdit(WidgetTextEdit *w)
{

    qScriptRegisterMetaType(this, WidgetFile::toScriptValue, WidgetFile::fromScriptValue);
    qScriptRegisterMetaType(this, WidgetTextEdit::toScriptValue, WidgetTextEdit::fromScriptValue);

    _widgetTextEdit = w;
    QScriptValue scriptTextEdit = this->newQObject((_widgetTextEdit));
    this->globalObject().setProperty("editor", scriptTextEdit);
    WidgetFile * file = _widgetTextEdit->widgetFile();
    MainWindow * win = file->window();
    QScriptValue scriptWindow = this->newQObject(win);
    this->globalObject().setProperty("window", scriptWindow);

    QScriptValue scriptCursor = this->newQObject(new ScriptCursor(w));
    this->globalObject().setProperty("cursor", scriptCursor);
}
