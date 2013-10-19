#include "filemanager.h"
#include "widgettextedit.h"
#include "widgetpdfdocument.h"
#include "widgetpdfviewer.h"
#include "syntaxhighlighter.h"
#include "configmanager.h"
#include "builder.h"
#include <QAction>
#include <QDebug>


FileManager FileManager::Instance;

FileManager::FileManager(QObject *parent) :
    QObject(parent),
    _currentWidgetFileId(-1)
{
    _pdfSynchronized = ConfigManager::Instance.isPdfSynchronized();
}

bool FileManager::newFile()
{
    WidgetFile * oldFile = this->currentWidgetFile();
    WidgetFile * newFile = new WidgetFile();
    newFile->initTheme();
    _widgetFiles.append(newFile);
    _currentWidgetFileId = _widgetFiles.count() - 1;
    changeConnexions(oldFile);
    return true;
}
void FileManager::changeConnexions(WidgetFile * oldFile)
{
    // Disconnect
    if(oldFile)
    {
        disconnect(oldFile, SIGNAL(verticalSplitterChanged()), this, SLOT(sendVerticalSplitterChanged()));
        disconnect(oldFile->widgetTextEdit(), SIGNAL(cursorPositionChanged(int,int)), this, SLOT(sendCursorPositionChanged(int,int)));
        disconnect(oldFile->widgetTextEdit()->getCurrentFile()->getBuilder(), SIGNAL(statusChanged(QString)), this, SLOT(sendMessageFromCurrentFile(QString)));
    }

    // Connect
    if(_pdfSynchronized)
    {
        connect(this->currentWidgetFile()->widgetTextEdit()->verticalScrollBar(),SIGNAL(valueChanged(int)), this->currentWidgetFile()->widgetPdfViewer()->widgetPdfDocument(),SLOT(jumpToPdfFromSourceView(int)));
    }
    connect(this->currentWidgetFile(), SIGNAL(verticalSplitterChanged()), this, SLOT(sendVerticalSplitterChanged()));
    connect(this->currentWidgetFile()->widgetTextEdit(), SIGNAL(cursorPositionChanged(int,int)), this, SLOT(sendCursorPositionChanged(int,int)));
    connect(this->currentWidgetFile()->widgetTextEdit()->getCurrentFile()->getBuilder(), SIGNAL(statusChanged(QString)), this, SLOT(sendMessageFromCurrentFile(QString)));

}

bool FileManager::open(QString filename)
{
    bool newWidget = this->newFile();
    this->currentWidgetFile()->open(filename);
    return newWidget;
}
void FileManager::openAssociatedFile()
{
    QAction * action = dynamic_cast<QAction*>(sender());
    QString filename = action->property("filename").toString();
    if(!filename.isEmpty())
    {
        emit requestOpenFile(filename);
    }

}

void FileManager::undo()
{
    this->currentWidgetFile()->widgetTextEdit()->undo();
}

void FileManager::redo()
{
    this->currentWidgetFile()->widgetTextEdit()->redo();
}
void FileManager::copy()
{
    this->currentWidgetFile()->widgetTextEdit()->copy();
}
void FileManager::cut()
{
    this->currentWidgetFile()->widgetTextEdit()->cut();
}
void FileManager::paste()
{
    this->currentWidgetFile()->widgetTextEdit()->paste();
}
void FileManager::wrapEnvironment()
{
    this->currentWidgetFile()->widgetTextEdit()->wrapEnvironment();
}
void FileManager::jumpToPdfFromSource()
{
    this->currentWidgetFile()->widgetPdfViewer()->widgetPdfDocument()->jumpToPdfFromSource();
}
void FileManager::rehighlight()
{
    foreach(WidgetFile * widgetFile, _widgetFiles)
    {
        widgetFile->syntaxHighlighter()->rehighlight();
        widgetFile->widgetTextEdit()->onCursorPositionChange();
    }
}
void FileManager::toggleConsole()
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->toggleConsole();
    }
}
void FileManager::toggleErrorTable()
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->toggleErrorTable();
    }
}
void FileManager::initTheme()
{
    foreach(WidgetFile * widgetFile, _widgetFiles)
    {
        widgetFile->initTheme();
    }
}

void FileManager::close(WidgetFile *widget)
{
    int id = _widgetFiles.indexOf(widget);

    if(_currentWidgetFileId >= id && _currentWidgetFileId != 0)
    {
        --_currentWidgetFileId;
    }
    _widgetFiles.removeOne(widget);
    delete widget;
}
void FileManager::setPdfSynchronized(bool pdfSynchronized)
{
    if(_pdfSynchronized != pdfSynchronized)
    {
        if(pdfSynchronized)
        {
            foreach(WidgetFile * widgetFile, _widgetFiles)
            {
                connect(widgetFile->widgetTextEdit()->verticalScrollBar(),SIGNAL(valueChanged(int)), widgetFile->widgetPdfViewer()->widgetPdfDocument(),SLOT(jumpToPdfFromSourceView(int)));
            }
        }
        else
        {
            foreach(WidgetFile * widgetFile, _widgetFiles)
            {
                disconnect(widgetFile->widgetTextEdit()->verticalScrollBar(),SIGNAL(valueChanged(int)), widgetFile->widgetPdfViewer()->widgetPdfDocument(),SLOT(jumpToPdfFromSourceView(int)));
            }
        }
    }
    _pdfSynchronized = pdfSynchronized;

}
