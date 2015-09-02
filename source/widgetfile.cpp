#include "widgetfile.h"
#include "hunspell/hunspell.hxx"
#include "minisplitter.h"
#include "widgettextedit.h"
#include "widgetconsole.h"
#include "taskpane/taskwindow.h"
#include "widgetfindreplace.h"
#include "widgetpdfdocument.h"
#include "widgetpdfviewer.h"
#include "widgetlinenumber.h"
#include "syntaxhighlighter.h"
#include "textdocument.h"
#include "textdocumentlayout.h"
#include "file.h"
#include "filemanager.h"
#include "builder.h"
#include "configmanager.h"
#include "tools.h"
#include "svnhelper.h"
#include "ipane.h"

#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QAction>
#include <QDebug>
#include <QTextCodec>
#include "mainwindow.h"

WidgetFile::WidgetFile(MainWindow *parent) :
    QWidget(0),
    _window(parent)
{
    _currentPane = 0;
    _masterFile = 0;
    TextDocument * doc = new TextDocument();
    TextDocumentLayout * doclayout = new TextDocumentLayout(doc);
    doc->setDocumentLayout(doclayout);
    _widgetTextEdit     = new WidgetTextEdit(this);
    _widgetTextEdit->setDocument(doc);
    _syntaxHighlighter  = new SyntaxHighlighter(this);
    _widgetTextEdit     ->setSyntaxHighlighter(_syntaxHighlighter);
    _widgetPdfViewer    = new WidgetPdfViewer();
    _widgetPdfViewer    ->widgetPdfDocument()->setWidgetFile(this);
    _widgetFindReplace  = new WidgetFindReplace(_widgetTextEdit);
     this->closeFindReplaceWidget();
    _widgetLineNumber   = new WidgetLineNumber(this);
    _widgetLineNumber   ->setWidgetTextEdit(_widgetTextEdit);
    _widgetTextEdit     ->setWidgetLineNumber(_widgetLineNumber);

    _widgetSimpleOutput = new TaskWindow();
    _widgetSimpleOutput ->setWidgetTextEdit(_widgetTextEdit);
    _widgetSimpleOutput->hideCategory("error");
    _widgetSimpleOutput->setStatusbarText(trUtf8("Warning"));
    _panes.prepend(_widgetSimpleOutput);

    _warningPane = new TaskWindow();
    _warningPane ->setWidgetTextEdit(_widgetTextEdit);
    _warningPane->hideCategory("warning");
    _warningPane->hideCategory("notice");
    _warningPane->openPaneOnError(true);
    _warningPane->setStatusbarText(trUtf8("Erreurs"));
    _panes.prepend(_warningPane);

    _panes.prepend(new WidgetConsole(this));


    _horizontalSplitter = new MiniSplitter(Qt::Horizontal);
    _verticalSplitter = new MiniSplitter(Qt::Vertical);


    _warningPaneHeight = _consoleHeight = _problemsHeight = 70;


    QGridLayout * layout = new QGridLayout();
    //layout->addWidget(_widgetLineNumber,0,0);
    layout->addWidget(_horizontalSplitter,0,0);
    layout->setColumnMinimumWidth(0,40);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
    this->setContentsMargins(0,8,0,0);

    _horizontalSplitter->addWidget(_verticalSplitter);
    if(!ConfigManager::Instance.pdfViewerInItsOwnWidget())
    {
        _horizontalSplitter->addWidget(_widgetPdfViewer);
    }

    QList<int> sizes;
    sizes << width()/2 << width()/2;
    _horizontalSplitter->setSizes(sizes);



    _widgetTextEdit2 = new WidgetTextEdit(this);
    _widgetTextEdit2->setDocument(doc);
    WidgetLineNumber * eLineNumber = new WidgetLineNumber(this);
    _widgetTextEdit2->setSyntaxHighlighter(_syntaxHighlighter);
    //_widgetTextEdit2->setDocument(_widgetTextEdit->document());
    eLineNumber->setWidgetTextEdit(_widgetTextEdit2);
   _widgetTextEdit2->setWidgetLineNumber(eLineNumber);



    _editorSplitter = new MiniSplitter(Qt::Vertical);
    _editorSplitter->setHandleWidth(4);
    QHBoxLayout * editorLayout;
    QWidget * w;


    editorLayout = new QHBoxLayout(_verticalSplitter);
    editorLayout->setContentsMargins(5,0,0,0);
    editorLayout->setSpacing(0);
    editorLayout->addWidget(eLineNumber);
    editorLayout->addWidget(_widgetTextEdit2);
    w =  new QWidget();
    w->setLayout(editorLayout);
    _editorSplitter->addWidget(w);


    editorLayout = new QHBoxLayout(_verticalSplitter);
    editorLayout->setContentsMargins(5,0,0,0);
    editorLayout->setSpacing(0);
    editorLayout->addWidget(this->_widgetLineNumber);
    editorLayout->addWidget(this->_widgetTextEdit);
    w =  new QWidget();
    w->setLayout(editorLayout);
    _editorSplitter->addWidget(w);


    _horizontalSplitter->setHandleWidth(1);
    _horizontalSplitter->setBackgroundColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color());
    _editorSplitter->setBackgroundColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color());
    _verticalSplitter->addWidget(_editorSplitter);

    foreach(IPane * pane, _panes)
    {
        _verticalSplitter->addWidget(pane->paneWidget());
        pane->paneWidget()->hide();
    }

    //_verticalSplitter->addWidget(this->_widgetFindReplace);
    //_verticalSplitter->addWidget(this->_widgetSimpleOutput->outputWidget());
    //_verticalSplitter->addWidget(this->_warningPane->outputWidget());
    //_verticalSplitter->addWidget(this->_widgetConsole->paneWidget());

    //_verticalSplitter->setCollapsible(4,true);
    //_verticalSplitter->setCollapsible(3,true);
    //_verticalSplitter->setCollapsible(2,true);


    connect(_widgetFindReplace->pushButtonClose(), SIGNAL(clicked()), this, SLOT(closeFindReplaceWidget()));
    connect(_widgetTextEdit,SIGNAL(textChanged()),_widgetLineNumber,SLOT(update()));
    connect(_widgetTextEdit->getCurrentFile()->builder(), SIGNAL(pdfChanged()),_widgetPdfViewer->widgetPdfDocument(),SLOT(updatePdf()));


    foreach(IPane * pane, _panes)
    {
        const QMetaObject * o = pane->getQObject()->metaObject();
        if(o->indexOfSignal("requestLine(int)") != -1)
        {
            connect(pane->getQObject(), SIGNAL(requestLine(int)), _widgetTextEdit, SLOT(goToLine(int)));
        }
    }




    connect(doc,SIGNAL(modificationChanged(bool)), this->file(),SLOT(setModified(bool)));
    connect(doc,SIGNAL(modificationChanged(bool)), _widgetLineNumber, SLOT(update()));
    connect(this->file(), SIGNAL(modified(bool)), doc,SLOT(setModified(bool)));
    connect(doclayout, SIGNAL(documentSizeChanged(QSizeF)), widgetTextEdit(), SLOT(adjustScrollbar(QSizeF)));
    connect(doclayout, SIGNAL(documentSizeChanged(QSizeF)), widgetTextEdit2(), SLOT(adjustScrollbar(QSizeF)));


    _widgetTextEdit->getCurrentFile()->create();
    setDictionary(ConfigManager::Instance.currentDictionary());
    _widgetTextEdit->setText(" ");

    _widgetPdfViewer->widgetPdfDocument()->setFile(_widgetTextEdit->getCurrentFile());
    _widgetSimpleOutput->setBuilder(_widgetTextEdit->getCurrentFile()->builder());
    _warningPane->setBuilder(_widgetTextEdit->getCurrentFile()->builder());
    _widgetTextEdit->selectAll();
    _widgetTextEdit->textCursor().setBlockCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
    QTextCursor cur(_widgetTextEdit->textCursor());
    cur.deletePreviousChar();
    _widgetTextEdit->setTextCursor(cur);
    _widgetTextEdit->getCurrentFile()->setModified(false);


    this->splitEditor(false);
}

WidgetFile::~WidgetFile()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete WidgetFile";
#endif
}


void WidgetFile::initTheme()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, ConfigManager::Instance.getTextCharFormats("line-number").background().color());
    this->setAutoFillBackground(true);
    this->setPalette(Pal);

    _widgetFindReplace->initTheme();
    _widgetTextEdit->initTheme();
    _widgetTextEdit2->initTheme();
    {
        QPalette Pal(palette());
        // set black background
        QTextCharFormat format = ConfigManager::Instance.getTextCharFormats("line-number");
        QBrush brush = format.background();
        Pal.setColor(QPalette::Background, brush.color());
        _widgetLineNumber->setAutoFillBackground(true);
        _widgetLineNumber->setPalette(Pal);
    }
}

File * WidgetFile::file()
{
     return widgetTextEdit()->getCurrentFile();
}

void WidgetFile::addWidgetPdfViewerToSplitter()
{
    //if(_horizontalSplitter->count()>1)
    {
        _horizontalSplitter->addWidget(_widgetPdfViewer);
        QList<int> sizes;
        sizes << width()/2 << width()/2;
        _horizontalSplitter->setSizes(sizes);
    }
}

void WidgetFile::openFindReplaceWidget()
{
    this->_widgetFindReplace->setMaximumHeight(110);
    this->_widgetFindReplace->setMinimumHeight(110);
    this->_widgetFindReplace->open();
}

void WidgetFile::closeFindReplaceWidget()
{
    this->_widgetFindReplace->setMaximumHeight(0);
    this->_widgetFindReplace->setMinimumHeight(0);
}

void WidgetFile::builTex(QString command)
{
    if(this->masterFile())
    {
        this->masterFile()->file()->save(true);
    }
    else
    {
        save();
    }
    file()->builder()->builTex(command);
}
void WidgetFile::bibtex()
{
    if(this->masterFile())
    {
        this->masterFile()->file()->save(true);
    }
    else
    {
        save();
    }
    file()->builder()->bibtex();
}
void WidgetFile::clean()
{
    file()->builder()->clean();
}
bool WidgetFile::isEmpty()
{
    return !_widgetTextEdit->getCurrentFile()->isModified() && _widgetTextEdit->toPlainText().isEmpty();
}

void WidgetFile::save()
{
    if(_widgetTextEdit->getCurrentFile()->getFilename().isEmpty())
    {
        return this->saveAs();
    }
    file()->save();
    //this->statusBar()->showMessage(tr(QString::fromUtf8("Sauvegardé").toLatin1()),2000);
}

void WidgetFile::saveAs(QString filename)
{
    if(filename.isEmpty())
    {
        filename = QFileDialog::getSaveFileName(this,tr("Enregistrer un fichier"), ConfigManager::Instance.lastFolder(),ConfigManager::Extensions);
    }
    if(filename.isEmpty())
    {
        return;
    }
    QString oldFilename = _widgetTextEdit->getCurrentFile()->getFilename();
    file()->save(filename);

    if(oldFilename.compare(filename))
    {
        emit FileManager::Instance.sendFilenameChanged(this, filename);
    }
}
void WidgetFile::reload()
{
    if(!this->file()->isUntitled())
    {
        this->open(this->file()->getFilename());
    }
}

void WidgetFile::open(QString filename)
{
    Tools::Log("WidgetFile::open: _widgetTextEdit->getCurrentFile()->open()");
    _widgetTextEdit->getCurrentFile()->open(filename);
    Tools::Log("WidgetFile::open: _widgetPdfViewer->widgetPdfDocument()->setFile()");
    _widgetPdfViewer->widgetPdfDocument()->setFile(_widgetTextEdit->getCurrentFile());


    _widgetTextEdit->document()->resetRevisions();

    if(ConfigManager::Instance.isSvnEnable()) {
        connect(new SvnHelper(filename), SIGNAL(uncommittedLines(QList<int>)), widgetTextEdit()->document(), SLOT(setUncommittedLines(QList<int>)));
    }

    Tools::Log("WidgetFile::open: resolve tex directives");
    if(file()->texDirectives().contains("spellcheck"))
    {
        this->setDictionary(file()->texDirectives().value("spellcheck"));
    }

    foreach(QAction * a, this->actions())
    {
        this->removeAction(a);
    }

    if(file()->texDirectives().contains("root"))
    {
        QString rootfile = file()->rootFilename();

        QString basename = rootfile;
        basename.replace(QRegExp("^.*[\\\\\\//]([^\\\\\\//]+)$"), "\\1");
        QAction * a = new QAction(trUtf8("Ouvrir ") + basename, this);
        a->setProperty("filename", rootfile);
        connect(a, SIGNAL(triggered()), &FileManager::Instance, SLOT(openAssociatedFile()));
        this->addAction(a);
    }

    foreach(AssociatedFile associatedfile,  _widgetTextEdit->getCurrentFile()->associatedFiles())
    {
        QString basename = associatedfile.filename;
        basename.replace(QRegExp("^.*[\\\\\\//]([^\\\\\\//]+)$"), "\\1");
        QAction * a = new QAction(trUtf8("Ouvrir ") + basename, this);
        a->setProperty("filename", associatedfile.filename);
        connect(a, SIGNAL(triggered()), &FileManager::Instance, SLOT(openAssociatedFile()));
        this->addAction(a);
    }
    emit opened();
}

void WidgetFile::setFileToBuild(File *file)
{
    widgetTextEdit()->getCurrentFile()->builder()->setFile(file);
}


Hunspell * WidgetFile::spellChecker()
{
    return _spellChecker;
}

QString WidgetFile::spellCheckerEncoding()
{
    if(!spellChecker())
    {
        return "";
    }
    return spellChecker()->get_dic_encoding();
}
void WidgetFile::setDictionary(QString dico)
{
    _dictionary = dico;
    if(dico == ConfigManager::NoDictionnary)
    {
        _spellChecker = 0;
    }
    else
    {
        _spellChecker = new Hunspell((ConfigManager::Instance.dictionaryPath() + _dictionary).toLatin1()+".aff",
                                     (ConfigManager::Instance.dictionaryPath() + _dictionary).toLatin1()+".dic");
        QTextCodec *codec = QTextCodec::codecForName(spellCheckerEncoding().toLatin1());
        foreach(const QString & word, ConfigManager::Instance.userDictionnary(_dictionary))
        {
            _spellChecker->add(codec->fromUnicode(word).data());
        }
    }
    bool modified = file()->isModified();
    syntaxHighlighter()->rehighlight();
    file()->setModified(modified);
    widgetTextEdit()->onCursorPositionChange();
}

void WidgetFile::closeCurrentPane()
{
    if(_currentPane)
    {
        closePane(_currentPane);
    }
}

void WidgetFile::openPane(IPane * pane)
{
    closeCurrentPane();
    pane->paneWidget()->show();
    _currentPane = pane;
    if(!pane->action()->isChecked())
    {
        pane->action()->toggle();
    }
}

void WidgetFile::closePane(IPane * pane)
{
    pane->paneWidget()->hide();
    _currentPane = 0;
    if(pane->action()->isChecked())
    {
        pane->action()->toggle();
    }
}

void WidgetFile::togglePane(IPane * pane)
{
    if(isPaneOpen(pane))
    {
        closePane(pane);
    }
    else
    {
        openPane(pane);
    }
}

bool WidgetFile::isPaneOpen(IPane * pane)
{
    return _currentPane == pane;
}


bool WidgetFile::isEditorSplited()
{
    return _editorSplitter->sizes().first();
}
void WidgetFile::splitEditor(bool split)
{
    if(QAction * splitEditorAction = this->window()->actionByRole("splitEditor"))
    {
        splitEditorAction->setChecked(split);
    }
    else
    {
        qDebug()<<"action not found";
    }
    if(!_editorSplitter->height() || !_editorSplitter->sizes().at(1))
    {
        QList<int> sizes;
        sizes << (int)split << 1;
        _editorSplitter->setCollapsible(0, !split);
        _editorSplitter->setSizes(sizes);
        if(split)
        {
            _editorSplitter->setBackgroundColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color());
        }
        else
        {
            _editorSplitter->setBackgroundColor(QColor(0,0,0,0));
        }
        return;
    }
    QList<int> sizes = _editorSplitter->sizes();
    if(split == (bool)sizes.first())
    {
        //already split or not split
        return;
    }
    if(split)
    {
        _editorSplitter->setBackgroundColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color());
        _editorSplitter->setHandleWidth(4);
        sizes[0] = _editorSplitter->height()/2;
        sizes[1] = _editorSplitter->height()/2;
        _editorSplitter->setCollapsible(0, false);
        _editorSplitter->setSizes(sizes);
        return;
    }
    _editorSplitter->setBackgroundColor(QColor(0,0,0,0));
    _editorSplitter->setHandleWidth(0);
    _editorSplitter->setCollapsible(0, true);
    sizes[0] = 0;
    sizes[1] = _editorSplitter->height();
    _editorSplitter->setSizes(sizes);
}
