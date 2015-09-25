#include "filemanager.h"
#include "widgettextedit.h"
#include "widgetpdfdocument.h"
#include "widgetpdfviewer.h"
#include "syntaxhighlighter.h"
#include "configmanager.h"
#include "builder.h"
#include "mainwindow.h"
#include "tools.h"
#include <QAction>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>

FileManager FileManager::Instance;

FileManager::FileManager(QObject *parent) :
    QObject(parent),
    _askReloadMessageBox(0),
    _currentWidgetFileId(-1),
    _mainWindow(0),
    _widgetPdfViewerWrapper(0)
{
}
void FileManager::init()
{
    _pdfSynchronized = ConfigManager::Instance.isPdfSynchronized();
    if(ConfigManager::Instance.pdfViewerInItsOwnWidget())
    {
        //initWidgetPdfViewerWrapper();
    }
    connect(&ConfigManager::Instance, SIGNAL(changed()), this, SLOT(updateLineWrapMode()));
}

void FileManager::initWidgetPdfViewerWrapper()
{
    _widgetPdfViewerWrapper = new WidgetPdfViewerWrapper();
    _widgetPdfViewerWrapper->setLayout(new QVBoxLayout());
    _widgetPdfViewerWrapper->setMinimumHeight(50);
    _widgetPdfViewerWrapper->setMinimumWidth(50);
    _widgetPdfViewerWrapper->layout()->setMargin(0);
    QSettings settings;
    _widgetPdfViewerWrapper->setGeometry(settings.value("pdfViewerWrapper/geometry", QRect(200,100,800,600)).toRect());

    _widgetPdfViewerWrapper->show();
}

bool FileManager::newFile(MainWindow * mainWindow)
{
    WidgetFile * oldFile = this->currentWidgetFile();
    WidgetFile * newFile = new WidgetFile(mainWindow);
    newFile->initTheme();
    _widgetFiles.append(newFile);
    _currentWidgetFileId = _widgetFiles.count() - 1;
    Q_ASSERT(_currentWidgetFileId >= 0);
    Tools::Log("FileManager::newFile: set _currentWidgetFileId to "+QString::number(_currentWidgetFileId));
    changeConnexions(oldFile);

    connect(newFile->file(), SIGNAL(modified(bool)), this, SLOT(sendCurrentFileModified(bool)));
    connect(newFile->file()->builder(), SIGNAL(pdfChanged()), this, SLOT(ensurePdfViewerIsVisible()));
    return true;
}
void FileManager::changeConnexions(WidgetFile * /*oldFile*/)
{
    // Disconnect
    /*if(oldFile)
    {
        disconnect(oldFile, SIGNAL(verticalSplitterChanged()), this, SLOT(sendVerticalSplitterChanged()));
        disconnect(oldFile->widgetTextEdit(), SIGNAL(cursorPositionChanged(int,int)), this, SLOT(sendCursorPositionChanged(int,int)));
        disconnect(oldFile->widgetTextEdit()->getCurrentFile()->builder(), SIGNAL(statusChanged(QString)), this, SLOT(sendMessageFromCurrentFile(QString)));
    }*/

    // Connect
    if(_pdfSynchronized)
    {
        connect(this->currentWidgetFile()->widgetTextEdit()->verticalScrollBar(),SIGNAL(valueChanged(int)), this->currentWidgetFile()->widgetPdfViewer()->widgetPdfDocument(),SLOT(jumpToPdfFromSourceView(int)));
    }
    connect(this->currentWidgetFile(), SIGNAL(verticalSplitterChanged()), this, SLOT(sendVerticalSplitterChanged()));
    connect(this->currentWidgetFile()->widgetTextEdit(), SIGNAL(cursorPositionChanged(int,int)), this, SLOT(sendCursorPositionChanged(int,int)));
    connect(this->currentWidgetFile()->widgetTextEdit()->getCurrentFile()->builder(), SIGNAL(statusChanged(QString)), this, SLOT(sendMessageFromCurrentFile(QString)));

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

void FileManager::createMasterConnexions(WidgetFile * widget, WidgetFile * master, AssociatedFile::Type type)
{
    master->file()->addOpenAssociatedFile(widget->file());

    // update the child
    connect(master->file()->builder(), SIGNAL(pdfChanged()),widget->widgetPdfViewer()->widgetPdfDocument(),SLOT(updatePdf()));

    widget->setMasterFile(master);
    widget->file()->builder()->setFile(master->file());

    // update the parent
    connect(widget->file()->builder(), SIGNAL(pdfChanged()),master->widgetPdfViewer()->widgetPdfDocument(),SLOT(updatePdf()));
}

void FileManager::deleteMasterConnexions(WidgetFile *widget, AssociatedFile::Type /*type*/)
{
    // if it has a master file
    // remove connexion with the master file
    if(widget->masterFile())
    {
        widget->masterFile()->file()->removeOpenAssociatedFile(widget->file());
    }

    // if it is a master file
    // restore everything with the opened associatedFiles
    foreach(File * openAssoc, widget->file()->openAssociatedFiles())
    {

        openAssoc->builder()->setFile(openAssoc);
        openAssoc->widgetFile()->setMasterFile(0);
    }

}

bool FileManager::open(QString filename, MainWindow * window)
{
    Tools::Log("FileManager::open: this->newFile()");
    bool newWidget = this->newFile(window);
    Tools::Log("FileManager::open: this->currentWidgetFile()->open()");
    this->currentWidgetFile()->open(filename);

    Tools::Log("FileManager::open: associate files");
    // is it an associated file?
    int masterId = 0;
    AssociatedFile assoc = this->reverseAssociation(filename, &masterId);
    if(masterId != -1)
    {
        WidgetFile * masterFile = this->widgetFile(masterId);
        if(assoc.type == AssociatedFile::INPUT && !this->currentWidgetFile()->file()->texDirectives().contains("root"))
        {
            this->currentWidgetFile()->file()->setRootFilename(masterFile->file()->getFilename());
            this->currentWidgetFile()->widgetPdfViewer()->widgetPdfDocument()->updatePdf();
        }
        createMasterConnexions(this->currentWidgetFile(), masterFile, assoc.type);
    }

    // is there already an opened associated file
    WidgetFile * associatedWidget;
    foreach(AssociatedFile associatedFile, this->currentWidgetFile()->file()->associatedFiles())
    {
        if((associatedWidget = this->widgetFile(associatedFile.filename)))
        {
            if(associatedFile.type == AssociatedFile::INPUT && !associatedWidget->file()->texDirectives().contains("root"))
            {
                associatedWidget->file()->setRootFilename(this->currentWidgetFile()->file()->getFilename());
                associatedWidget->widgetPdfViewer()->widgetPdfDocument()->updatePdf();
            }

            createMasterConnexions(associatedWidget, this->currentWidgetFile(), associatedFile.type);
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
AssociatedFile FileManager::reverseAssociation(QString filename, int *index)
{
    int idx = 0;
    foreach(WidgetFile * widgetFile, _widgetFiles)
    {
        AssociatedFile assoc;
        if((assoc = widgetFile->file()->associationWith(filename)).type != AssociatedFile::NONE)
        {
            if(index) *index = idx;
            return assoc;
        }
        ++idx;
    }
    if(index) *index = -1;
    return AssociatedFile::NoAssociation;
}

void FileManager::goToSection()
{
    if(!currentWidgetFile())
    {
        return;
    }
    QAction * action = qobject_cast<QAction*>(sender());
    if(!action)
    {
        return;
    }
    currentWidgetFile()->widgetTextEdit()->goToSection(action->text().trimmed());
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
    if(index >= _widgetFiles.count())
    {
        index = -1;
    }
    _currentWidgetFileId = index;
    if(index == -1)
    {
        return;
    }

    setCurrentPdfToPdfViewer();
}
void FileManager::setCurrentPdfToPdfViewer()
{
    if(_widgetPdfViewerWrapper && this->currentWidgetFile())
    {
        while(_widgetPdfViewerWrapper->layout()->count())
        {
            _widgetPdfViewerWrapper->layout()->itemAt(0)->widget()->setParent(0);
            _widgetPdfViewerWrapper->layout()->removeItem(_widgetPdfViewerWrapper->layout()->itemAt(0));
        }
        _widgetPdfViewerWrapper->layout()->addWidget(this->currentWidgetFile()->widgetPdfViewer());
    }
}

void FileManager::openCurrentPdf()
{
    if(!this->currentWidgetFile())
    {
        return;
    }
    QFileInfo pdfFile(this->currentWidgetFile()->file()->getPdfFilename());
    if(!pdfFile.exists())
    {
        return;
    }
    QDesktopServices::openUrl(QUrl("file:///"+pdfFile.absoluteFilePath()));
}

File * FileManager::file(int index)
{
    return _widgetFiles.at(index)->file();
}
void FileManager::updateLineWrapMode()
{
    foreach(WidgetFile* widget, _widgetFiles)
    {
        widget->widgetTextEdit()->updateLineWrapMode();
    }
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
void FileManager::jumpToPdfFromSource()
{
    if(this->currentWidgetFile())
        this->currentWidgetFile()->widgetPdfViewer()->widgetPdfDocument()->jumpToPdfFromSource();
}
void FileManager::rehighlight()
{
    foreach(WidgetFile * widgetFile, _widgetFiles)
    {
        bool modified = widgetFile->file()->isModified();
        widgetFile->syntaxHighlighter()->rehighlight();
        widgetFile->widgetTextEdit()->onCursorPositionChange();
        widgetFile->file()->setModified(modified); //because rehighlight call somehow setModify(true) :(
    }
}
void FileManager::setEncoding(QString codec)
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->file()->setCodec(codec);
    }
}
void FileManager::comment()
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->widgetTextEdit()->comment();
    }
}
void FileManager::uncomment()
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->widgetTextEdit()->uncomment();
    }
}
void FileManager::toggleComment()
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->widgetTextEdit()->toggleComment();
    }
}
void FileManager::checkGrammar()
{
    if(this->currentWidgetFile())
    {
        this->currentWidgetFile()->widgetTextEdit()->checkGrammar();
    }
}
void FileManager::reopenWithEncoding(QString codec)
{
    if(this->currentWidgetFile())
    {
        if(this->currentWidgetFile()->file()->isModified())
        {
            return;
        }
        this->currentWidgetFile()->file()->setCodec(codec);
        this->currentWidgetFile()->reload();
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
    widget->setVisible(false);
    int id = _widgetFiles.indexOf(widget);
    if(id == -1)
    {
        return;
    }
    this->deleteMasterConnexions(widget);
    widget->addWidgetPdfViewerToSplitter();
    if(_currentWidgetFileId >= id && _currentWidgetFileId != 0)
    {
        --_currentWidgetFileId;
    }
    _widgetFiles.removeOne(widget);
    widget->deleteLater();
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
void FileManager::setPdfViewerInItsOwnWidget(bool ownWidget)
{
    if(ownWidget)
    {
        if(!_widgetPdfViewerWrapper)
        {
            initWidgetPdfViewerWrapper();
        }
        this->setCurrent(_currentWidgetFileId);
    }
    else
    {
        foreach(WidgetFile * widgetFile, _widgetFiles)
        {
            widgetFile->addWidgetPdfViewerToSplitter();
        }
        if(_widgetPdfViewerWrapper)
        {
            _widgetPdfViewerWrapper->close();
            _widgetPdfViewerWrapper->deleteLater();
            _widgetPdfViewerWrapper = 0;
        }
    }
}
void FileManager::ensurePdfViewerIsVisible()
{
    if(ConfigManager::Instance.pdfViewerInItsOwnWidget())
    {
        if(!_widgetPdfViewerWrapper)
        {
            initWidgetPdfViewerWrapper();
            setCurrentPdfToPdfViewer();
        }
        if(!_widgetPdfViewerWrapper->isVisible())
        {
            _widgetPdfViewerWrapper->show();
        }
        _widgetPdfViewerWrapper->raise();
    }
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
void FileManager::checkCurrentFileSystemChanges()
{
    if(!currentWidgetFile() || currentWidgetFile()->file()->isUntitled())
    {
        return;
    }
    if(currentWidgetFile()->file()->lastSaved()
            < currentWidgetFile()->file()->fileInfo().lastModified())
    {
        onFileSystemChanged(currentWidgetFile());
    }
}

void FileManager::onFileSystemChanged(QString filename)
{
    WidgetFile * w = widgetFile(filename);
    if(w)
    {
        onFileSystemChanged(w);
    }
}

void FileManager::onFileSystemChanged(WidgetFile * widget)
{
    QString filename = widget->file()->getFilename();


    if(!_askReloadMessageBox)
    {
         QString message = trUtf8("Le fichier %1 à été modifié en dehors de %2. Voulez-vous le charger à nouveau ?").arg(QFileInfo(filename).fileName()).arg(APPLICATION_NAME);

        _askReloadMessageBox = new QMessageBox(trUtf8("Un fichier à été modifié."),
                                              message,
            QMessageBox::Warning,
            QMessageBox::Yes | QMessageBox::Default,
            QMessageBox::No,
            QMessageBox::NoButton,
            _mainWindow, Qt::Sheet);
        _askReloadMessageBox->setButtonText(QMessageBox::Yes, trUtf8("Charger à nouveau"));
        _askReloadMessageBox->setButtonText(QMessageBox::No, trUtf8("Ignorer"));
    }
    if(_askReloadMutex.tryLock())
    {
        if(QMessageBox::Yes == _askReloadMessageBox->exec())
        {
            widget->reload();
        }
        _askReloadMutex.unlock();
    }


}

bool FileManager::handleMimeData(const QMimeData * mimeData)
{
    return _mainWindow->handleMimeData(mimeData);
}

bool FileManager::onMacroTriggered(Macro macro, bool force)
{
    if(currentWidgetFile())
    {
        return currentWidgetFile()->widgetTextEdit()->onMacroTriggered(macro, force);
    }
    return false;
}
