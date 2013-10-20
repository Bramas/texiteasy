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

    // is it an associated file?
    int masterId = this->reverseAssociatedFileIndex(filename);
    if(masterId != -1)
    {
        WidgetFile * masterFile = this->widgetFile(masterId);
        masterFile->file()->addOpenAssociatedFile(this->currentWidgetFile()->file());

        // update the child
        this->currentWidgetFile()->setMasterFile(masterFile);
        this->currentWidgetFile()->file()->getBuilder()->setFile(this->file(masterId));

        // update the parent
        connect(this->currentWidgetFile()->file()->getBuilder(), SIGNAL(pdfChanged()),masterFile->widgetPdfViewer()->widgetPdfDocument(),SLOT(updatePdf()));

    }


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
int FileManager::reverseAssociatedFileIndex(QString filename)
{
    int index = 0;
    foreach(WidgetFile * widgetFile, _widgetFiles)
    {
        if(widgetFile->file()->isAssociatedWith(filename))
        {
            return index;
        }
        ++index;
    }
    return -1;
}
File * FileManager::file(int index)
{
    return _widgetFiles.at(index)->file();
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

    // remove connexion with the master file
    if(widget->masterFile())
    {
        connect(widget->file()->getBuilder(), SIGNAL(pdfChanged()),widget->masterFile()->widgetPdfViewer()->widgetPdfDocument(),SLOT(updatePdf()));
    }
    // restore everything with the open associatedFiles
    foreach(File * openAssoc, widget->file()->openAssociatedFiles())
    {
        openAssoc->widgetFile()->setMasterFile(0);
        openAssoc->getBuilder()->setFile(openAssoc);
    }
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
