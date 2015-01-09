#include "widgetfile.h"
#include "hunspell/hunspell.hxx"
#include "minisplitter.h"
#include "widgettextedit.h"
#include "widgetconsole.h"
#include "widgetsimpleoutput.h"
#include "widgetfindreplace.h"
#include "widgetpdfdocument.h"
#include "widgetpdfviewer.h"
#include "widgetlinenumber.h"
#include "syntaxhighlighter.h"
#include "file.h"
#include "filemanager.h"
#include "builder.h"
#include "configmanager.h"
#include "tools.h"
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
    _masterFile = 0;
    _widgetTextEdit     = new WidgetTextEdit(this);
    _syntaxHighlighter  = new SyntaxHighlighter(this);
    _widgetTextEdit     ->setSyntaxHighlighter(_syntaxHighlighter);
    _widgetPdfViewer    = new WidgetPdfViewer();
    _widgetPdfViewer    ->widgetPdfDocument()->setWidgetTextEdit(_widgetTextEdit);
    _widgetFindReplace  = new WidgetFindReplace(_widgetTextEdit);
     this->closeFindReplaceWidget();
    _widgetLineNumber   = new WidgetLineNumber(this);
    _widgetLineNumber   ->setWidgetTextEdit(_widgetTextEdit);
    _widgetTextEdit     ->setWidgetLineNumber(_widgetLineNumber);
    _widgetConsole      = new WidgetConsole();
    _widgetSimpleOutput = new WidgetSimpleOutput(this);
    _widgetSimpleOutput ->setWidgetTextEdit(_widgetTextEdit);

    _horizontalSplitter = new MiniSplitter(Qt::Horizontal);
    _verticalSplitter = new MiniSplitter(Qt::Vertical);




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
    WidgetLineNumber * eLineNumber = new WidgetLineNumber(this);
    _widgetTextEdit2->setSyntaxHighlighter(_syntaxHighlighter);
    _widgetTextEdit2->setDocument(_widgetTextEdit->document());
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
    _verticalSplitter->addWidget(this->_widgetFindReplace);
    _verticalSplitter->addWidget(this->_widgetSimpleOutput);
    _verticalSplitter->addWidget(this->_widgetConsole);

    _verticalSplitter->setCollapsible(3,true);
    _verticalSplitter->setCollapsible(2,true);


    connect(_widgetFindReplace->pushButtonClose(), SIGNAL(clicked()), this, SLOT(closeFindReplaceWidget()));
    connect(_widgetTextEdit,SIGNAL(textChanged()),_widgetLineNumber,SLOT(update()));
    connect(_widgetTextEdit->getCurrentFile()->getBuilder(), SIGNAL(pdfChanged()),_widgetPdfViewer->widgetPdfDocument(),SLOT(updatePdf()));
    connect(_widgetTextEdit->getCurrentFile()->getBuilder(), SIGNAL(error()),this,SLOT(openErrorTable()));
    connect(_widgetTextEdit->getCurrentFile()->getBuilder(), SIGNAL(success()),this,SLOT(closeErrorTable()));
    connect(_widgetConsole, SIGNAL(requestLine(int)), _widgetTextEdit, SLOT(goToLine(int)));


    _widgetTextEdit->getCurrentFile()->create();
    setDictionary(ConfigManager::Instance.currentDictionary());
    _widgetTextEdit->setText(" ");

    _widgetPdfViewer->widgetPdfDocument()->setFile(_widgetTextEdit->getCurrentFile());
    _widgetConsole->setBuilder(_widgetTextEdit->getCurrentFile()->getBuilder());
    _widgetConsole->setMaximumHeight(0);
    _widgetSimpleOutput->setBuilder(_widgetTextEdit->getCurrentFile()->getBuilder());
    _widgetSimpleOutput->setMaximumHeight(0);
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

bool WidgetFile::isConsoleOpen()
{
    QList<int> sizes = this->verticalSplitter()->sizes();
    if(sizes[3] > 0)
    {
        return true;
    }
    return false;
}
bool WidgetFile::isErrorTableOpen()
{
    QList<int> sizes = this->verticalSplitter()->sizes();
    if(sizes[2] > 0)
    {
        return true;
    }
    return false;
}
void WidgetFile::openConsole()
{
    if(isConsoleOpen())
    {
        return;
    }
    if(isErrorTableOpen())
    {
        closeErrorTable();
    }
    QList<int> sizes = this->verticalSplitter()->sizes();
    sizes.replace(0, sizes[0] - 60 + sizes[3]);
    sizes.replace(2, 0);
    sizes.replace(3, 60);
    this->verticalSplitter()->widget(3)->setMaximumHeight(height()*2/3);
    this->verticalSplitter()->setSizes(sizes);
    emit verticalSplitterChanged();
}

void WidgetFile::openErrorTable()
{
    if(isErrorTableOpen())
    {
        return;
    }
    if(isConsoleOpen())
    {
        closeConsole();
    }
    QList<int> sizes = this->verticalSplitter()->sizes();
    sizes.replace(0, sizes[0] - 60 + sizes[3]);
    sizes.replace(2, 60);
    sizes.replace(3, 0);
    this->verticalSplitter()->widget(2)->setMaximumHeight(height()*2/3);
    this->verticalSplitter()->setSizes(sizes);
    emit verticalSplitterChanged();
}

void WidgetFile::closeConsole()
{
    if(!isConsoleOpen())
    {
        return;
    }
    QList<int> sizes = this->verticalSplitter()->sizes();
    sizes.replace(0, sizes[0] + sizes[3]);
    sizes.replace(3, 0);
    this->verticalSplitter()->widget(3)->setMaximumHeight(0);
    this->verticalSplitter()->setSizes(sizes);
    emit verticalSplitterChanged();
}

void WidgetFile::closeErrorTable()
{
    if(!isErrorTableOpen())
    {
        return;
    }
    QList<int> sizes = this->verticalSplitter()->sizes();
    sizes.replace(0, sizes[0] + sizes[2]);
    sizes.replace(2, 0);
    this->verticalSplitter()->widget(2)->setMaximumHeight(0);
    this->verticalSplitter()->setSizes(sizes);
    emit verticalSplitterChanged();
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

void WidgetFile::toggleConsole()
{
    if(isConsoleOpen())
    {
        closeConsole();
    }
    else
    {
        openConsole();
    }
}
void WidgetFile::toggleErrorTable()
{
    if(isErrorTableOpen())
    {
        closeErrorTable();
    }
    else
    {
        openErrorTable();
    }
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
    file()->getBuilder()->builTex(command);
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
    file()->getBuilder()->bibtex();
}
void WidgetFile::clean()
{
    file()->getBuilder()->clean();
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

void WidgetFile::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("Enregistrer un fichier"), ConfigManager::Instance.lastFolder(),ConfigManager::Extensions);
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
    Tools::Log("WidgetFile::open: _widgetConsole->setBuilder()");
    _widgetConsole->setBuilder(_widgetTextEdit->getCurrentFile()->getBuilder());


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
}

void WidgetFile::setFileToBuild(File *file)
{
    widgetTextEdit()->getCurrentFile()->getBuilder()->setFile(file);
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

void WidgetFile::splitEditor(bool split)
{
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
