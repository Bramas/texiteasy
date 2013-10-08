#include "widgetfile.h"
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
#include "builder.h"
#include "configmanager.h"
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QDebug>

WidgetFile::WidgetFile(QWidget *parent) :
    QWidget(parent)
{
    _widgetTextEdit = new WidgetTextEdit(this);
    _syntaxHighlighter = new SyntaxHighlighter(_widgetTextEdit->document());
    _widgetTextEdit->setSyntaxHighlighter(_syntaxHighlighter);
    _widgetPdfViewer = new WidgetPdfViewer(this);
    _widgetPdfViewer->widgetPdfDocument()->setWidgetTextEdit(_widgetTextEdit);
    _widgetFindReplace = new WidgetFindReplace(_widgetTextEdit);
     this->closeFindReplaceWidget();
    _widgetLineNumber = new WidgetLineNumber(this);
    _widgetLineNumber->setWidgetTextEdit(_widgetTextEdit);
    _widgetTextEdit->setWidgetLineNumber(_widgetLineNumber);
    _widgetConsole = new WidgetConsole();
    _widgetSimpleOutput = new WidgetSimpleOutput(this);
    _widgetSimpleOutput->setWidgetTextEdit(_widgetTextEdit);

    _horizontalSplitter = new MiniSplitter(Qt::Horizontal);
    _verticalSplitter = new MiniSplitter(Qt::Vertical);


    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widgetLineNumber,0,0);
    layout->addWidget(_horizontalSplitter,0,1);
    layout->setColumnMinimumWidth(0,40);
    this->setLayout(layout);

    _horizontalSplitter->addWidget(_verticalSplitter);
    _horizontalSplitter->addWidget(_widgetPdfViewer);


    _verticalSplitter->addWidget(this->_widgetTextEdit);
    _verticalSplitter->addWidget(this->_widgetFindReplace);
    _verticalSplitter->addWidget(this->_widgetSimpleOutput);
    _verticalSplitter->addWidget(this->_widgetConsole);

    _verticalSplitter->setCollapsible(3,true);
    _verticalSplitter->setCollapsible(2,true);


    connect(_widgetTextEdit->verticalScrollBar(),SIGNAL(valueChanged(int)), _widgetPdfViewer->widgetPdfDocument(),SLOT(jumpToPdfFromSourceView(int)));

    connect(_widgetFindReplace->pushButtonClose(), SIGNAL(clicked()), this, SLOT(closeFindReplaceWidget()));
    connect(_widgetTextEdit,SIGNAL(textChanged()),_widgetLineNumber,SLOT(update()));
    connect(_widgetTextEdit->getCurrentFile()->getBuilder(), SIGNAL(pdfChanged()),_widgetPdfViewer->widgetPdfDocument(),SLOT(updatePdf()));
    connect(_widgetConsole, SIGNAL(requestLine(int)), _widgetTextEdit, SLOT(goToLine(int)));


    _widgetTextEdit->getCurrentFile()->create();
    _widgetTextEdit->setText(" ");

    _widgetPdfViewer->widgetPdfDocument()->setFile(_widgetTextEdit->getCurrentFile());
    _widgetConsole->setBuilder(_widgetTextEdit->getCurrentFile()->getBuilder());
    _widgetSimpleOutput->setBuilder(_widgetTextEdit->getCurrentFile()->getBuilder());
    int pos = _widgetTextEdit->textCursor().position();
    _widgetTextEdit->selectAll();
    _widgetTextEdit->textCursor().setBlockCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
    QTextCursor cur(_widgetTextEdit->textCursor());
    cur.deletePreviousChar();
    _widgetTextEdit->setTextCursor(cur);
    _widgetTextEdit->getCurrentFile()->setModified(false);
}

WidgetFile::~WidgetFile()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete WidgetFile";
#endif
    //delete this->layout();
    //delete _verticalSplitter;
    //delete _horizontalSplitter;
    //delete _widgetSimpleOutput;
    //delete _widgetConsole;
    //delete _widgetLineNumber;
    //delete _widgetFindReplace;
    //delete _widgetPdfViewer;
    //delete _syntaxHighlighter;
    //delete _widgetTextEdit;
}

void WidgetFile::initTheme()
{
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, ConfigManager::Instance.getTextCharFormats("linenumber").background().color());
    this->setAutoFillBackground(true);
    this->setPalette(Pal);
    _widgetTextEdit->setStyleSheet(QString("QPlainTextEdit { border: 1px solid ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("textedit-border").foreground().color())+"; "+
                                        QString("color: ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+"; "+
                                        QString("background-color: ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color())+
                                "; }");
    _widgetTextEdit->setCurrentCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
    QTextCursor cur = _widgetTextEdit->textCursor();
    cur.setCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
    _widgetTextEdit->setTextCursor(cur);
    //this->widgetTextEdit->setCurrentFont(ConfigManager::Instance.getTextCharFormats("normal").font());


#ifdef OS_MAC
    if(ConfigManager::Instance.getTextCharFormats("normal").background().color().value()<100) // if it's a dark color
    {
        QPixmap whiteBeamPixmap("/Users/quentinbramas/Projects/texiteasy/texiteasy-repository/source/data/cursor/whiteBeam.png");
        QCursor whiteBeam(whiteBeamPixmap);
        _widgetTextEdit->viewport()->setCursor(whiteBeam);
    }
    else
    {
        _widgetTextEdit->viewport()->setCursor(Qt::IBeamCursor);
    }
#endif

    {
        QPalette Pal(palette());
        // set black background
        QTextCharFormat format = ConfigManager::Instance.getTextCharFormats("linenumber");
        QBrush brush = format.background();
        Pal.setColor(QPalette::Background, brush.color());
        _widgetLineNumber->setAutoFillBackground(true);
        _widgetLineNumber->setPalette(Pal);
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


void WidgetFile::pdflatex()
{
    this->save();
    _widgetTextEdit->getCurrentFile()->getBuilder()->pdflatex();
}
void WidgetFile::bibtex()
{
    this->save();
    _widgetTextEdit->getCurrentFile()->getBuilder()->bibtex();
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
    _widgetTextEdit->getCurrentFile()->setData(_widgetTextEdit->toPlainText());
    _widgetTextEdit->getCurrentFile()->save();
    //this->statusBar()->showMessage(tr(QString::fromUtf8("Sauvegardé").toLatin1()),2000);
}

void WidgetFile::saveAs()
{
    //this->widgetTextEdit->getCurrentFile()->setData("sdfsdfg");
    //return;
    QString filename = QFileDialog::getSaveFileName(this,tr("Enregistrer un fichier"));
    _widgetTextEdit->getCurrentFile()->setData(_widgetTextEdit->toPlainText());
    if(filename.isEmpty())
    {
        return;
    }
    _widgetTextEdit->getCurrentFile()->save(filename);
    this->setWindowTitle(filename.replace(QRegExp("^.*[\\\\\\/]([^\\\\\\/]*)$"),"\\1")+" - texiteasy");
}

void WidgetFile::open(QString filename)
{
    _widgetTextEdit->getCurrentFile()->open(filename);
    _widgetPdfViewer->widgetPdfDocument()->setFile(_widgetTextEdit->getCurrentFile());
    _widgetConsole->setBuilder(_widgetTextEdit->getCurrentFile()->getBuilder());
}
