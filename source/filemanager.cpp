#include "filemanager.h"
#include "widgettextedit.h"
#include "widgetpdfdocument.h"
#include "widgetpdfviewer.h"
#include "syntaxhighlighter.h"
#include "configmanager.h"
#include "builder.h"
#include "mainwindow.h"
#include <QAction>
#include <QDebug>
#include <QMessageBox>

FileManager FileManager::Instance;

FileManager::FileManager(QObject *parent) :
    QObject(parent),
    _currentWidgetFileId(-1),
    _mainWindow(0)
{
    _pdfSynchronized = ConfigManager::Instance.isPdfSynchronized();
    connect(&_fileSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileSystemChanged(QString)));
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

WidgetFile * FileManager::widgetFile(QString filename)
{
    foreach(WidgetFile * widget, _widgetFiles)
    {
        if(!filename.compare(widget->file()->getFilename()))
        {
            return widget;
        }
    }
    return 0;
}

void FileManager::createMasterConnexions(WidgetFile * widget, WidgetFile * master)
{
    master->file()->addOpenAssociatedFile(widget->file());

    // update the child
    widget->setMasterFile(master);
    widget->file()->getBuilder()->setFile(master->file());

    // update the parent
    connect(widget->file()->getBuilder(), SIGNAL(pdfChanged()),master->widgetPdfViewer()->widgetPdfDocument(),SLOT(updatePdf()));
}

void FileManager::deleteMasterConnexions(WidgetFile *widget)
{
    // if it has a master file
    // remove connexion with the master file
    if(widget->masterFile())
    {
        widget->masterFile()->file()->removeOpenAssociatedFile(widget->file());
        disconnect(widget->file()->getBuilder(), SIGNAL(pdfChanged()),widget->masterFile()->widgetPdfViewer()->widgetPdfDocument(),SLOT(updatePdf()));

        // the widget maybe own the pdfDocument of the master file
        // in this case we have to remove it as a child because
        // if the widget is deleted, all children are deleted
        if(widget->masterFile()->widgetPdfViewer()->widgetPdfDocument()->parent()
                == widget->widgetPdfViewer())
        {
            widget->masterFile()->widgetPdfViewer()->restorPdfDocumentParent();
        }
    }

    // if it is a master file
    // restore everything with the open associatedFiles
    foreach(File * openAssoc, widget->file()->openAssociatedFiles())
    {
        openAssoc->widgetFile()->setMasterFile(0);
        openAssoc->getBuilder()->setFile(openAssoc);
    }

}

bool FileManager::open(QString filename)
{
    bool newWidget = this->newFile();
    this->currentWidgetFile()->open(filename);
    _fileSystemWatcher.addPath(filename);

    // is it an associated file?
    int masterId = this->reverseAssociatedFileIndex(filename);
    if(masterId != -1)
    {
        WidgetFile * masterFile = this->widgetFile(masterId);
        createMasterConnexions(this->currentWidgetFile(), masterFile);
    }

    // is there already an opened associated file
    WidgetFile * associatedWidget;
    foreach(AssociatedFile associatedFile, this->currentWidgetFile()->file()->associatedFiles())
    {
        if(associatedWidget = this->widgetFile(associatedFile.filename))
        {
            createMasterConnexions(associatedWidget, this->currentWidgetFile());
        }
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

void FileManager::setCurrent(WidgetFile *widget)
{
    if(!widget)
    {
        setCurrent(-1);
        return;
    }
    setCurrent(_widgetFiles.indexOf(widget));
}
void FileManager::setCurrent(int index)
{
     _currentWidgetFileId = index;
     if(index == -1)
     {
         return;
     }

     if(this->currentWidgetFile()->masterFile())
     {
         this->currentWidgetFile()->widgetPdfViewer()->setWidgetPdfDocument(this->currentWidgetFile()->masterFile()->widgetPdfViewer()->widgetPdfDocument());
     }
     if(!this->currentWidgetFile()->file()->openAssociatedFiles().isEmpty())
     {
         this->currentWidgetFile()->widgetPdfViewer()->restorPdfDocumentParent();
     }
}

File * FileManager::file(int index)
{
    return _widgetFiles.at(index)->file();
}
void FileManager::undo()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetTextEdit()->undo();
}

void FileManager::redo()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetTextEdit()->redo();
}
void FileManager::copy()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetTextEdit()->copy();
}
void FileManager::cut()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetTextEdit()->cut();
}
void FileManager::paste()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetTextEdit()->paste();
}
void FileManager::wrapEnvironment()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetTextEdit()->wrapEnvironment();
}
void FileManager::jumpToPdfFromSource()
{
    if(this->currentWidgetFile())
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
    if(id == -1)
    {
        return;
    }
    deleteMasterConnexions(widget);
    _fileSystemWatcher.removePath(widget->file()->getFilename());
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
void FileManager::setDictionaryFromAction()
{
    if(!this->currentWidgetFile())
    {
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    if(!action)
    {
        return;
    }
    this->currentWidgetFile()->setDictionary(action->text());
}

void FileManager::builTex()
{
    QAction * action = qobject_cast<QAction*>(sender());
    if(!action)
    {
        return;
    }
    QString command = ConfigManager::Instance.latexCommand(action->text());
    if(!command.isEmpty())
    {
        this->currentWidgetFile()->builTex(command);
    }
}

void FileManager::onFileSystemChanged(QString filename)
{
    QString message = trUtf8("Le fichier %1 à été modifié en dehors de %2. Voulez-vous le charger à nouveau ?").arg(QFileInfo(filename).baseName()).arg(APPLICATION_NAME);
    if(0 == QMessageBox::warning(_mainWindow, trUtf8("Un fichier à été modifié."), message, trUtf8("Oui"), trUtf8("Non")))
    {
        WidgetFile * w = widgetFile(filename);
        if(w)
        {
            w->open(filename);
        }

    }
}
