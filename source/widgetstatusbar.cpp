#include "widgetstatusbar.h"
#include "ui_widgetstatusbar.h"
#include "configmanager.h"
#include "filemanager.h"
#include "minisplitter.h"

#include <QPushButton>
#include <QDebug>
#include <QSplitter>
#include <QToolButton>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

WidgetStatusBar::WidgetStatusBar(QWidget *parent) :
    QStatusBar(parent),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::PreventContextMenu);


   /* _pushButtonConsole = new QToolButton();
    _pushButtonConsole->setText(trUtf8("Console"));
    _pushButtonConsole->setCheckable(true);
    _pushButtonConsole->setChecked(sizes[3] != 0);
    _pushButtonConsole->setAutoRaise(true);
    _pushButtonConsole->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_pushButtonConsole, 0);
*/
    _labelConsole = new QLabel(QString("<div style='margin:5px;'><a class='link' style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>Console</a></div>");
    _labelConsole->setStyleSheet(QString("QLabel {  font-size:11px; }"));
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(QColor("#000000"));
    effect->setOffset(-1,-1);
    _labelConsole->setGraphicsEffect(effect);
    this->addPermanentWidget(_labelConsole);

    _labelErrorTable = new QLabel(QString("<div style='margin:5px;'><a style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>Erreurs</a></div>");
    _labelErrorTable->setStyleSheet(QString("a { font-size:11px; color:")+
                                 ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                                 QString("; text-decoration:none;")+
                                 QString("}")
                                 );
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(QColor("#000000"));
    effect->setOffset(-1,-1);
    _labelErrorTable->setGraphicsEffect(effect);
    this->addPermanentWidget(_labelErrorTable);

/*
    _pushButtonErreurs = new QToolButton();
    _pushButtonErreurs->setText(trUtf8("Erreurs"));
    _pushButtonErreurs->setCheckable(true);
    _pushButtonErreurs->setChecked(sizes[2] != 0);
    _pushButtonErreurs->setAutoRaise(true);
    _pushButtonErreurs->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_pushButtonErreurs, 0);
*/
    _positionLabel = new QLabel(trUtf8("Ligne %1, Colonne %2").arg("1").arg("1"),this);
    _positionLabel->setStyleSheet(QString("font-size:11px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())); 
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(QColor("#000000"));
    effect->setOffset(-1,-1);
    _positionLabel->setGraphicsEffect(effect);
    this->addPermanentWidget(_positionLabel, 0);

    QLabel* messageArea = new QLabel(this);
    messageArea->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    connect(this, SIGNAL(messageChanged(QString)), messageArea, SLOT(setText(QString)));

    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(QColor("#000000"));
    effect->setOffset(-1,-1);
    messageArea->setGraphicsEffect(effect);
    this->addPermanentWidget(messageArea, 1);


    _encodingLabel = new QLabel(this);
    _encodingLabel->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(QColor("#000000"));
    effect->setOffset(-1,-1);
    _encodingLabel->setGraphicsEffect(effect);
    this->addPermanentWidget(_encodingLabel, 0);

    //connect(_pushButtonConsole, SIGNAL(clicked()), this, SLOT(toggleConsole()));
    connect(_labelConsole, SIGNAL(linkActivated(QString)), this, SLOT(toggleConsole()));
    //connect(_pushButtonErreurs, SIGNAL(clicked()), this, SLOT(toggleErrorTable()));
    connect(_labelErrorTable, SIGNAL(linkActivated(QString)), this, SLOT(toggleErrorTable()));

    this->setStyleSheet("QStatusBar::item { border: none;} QStatusBar {padding:0; height:100px; background: "+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("linenumber").background().color())+
                                     "}");

    this->setMaximumHeight(20);
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}


void WidgetStatusBar::toggleConsole()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        return;
    }
    QList<int> sizes = FileManager::Instance.currentWidgetFile()->verticalSplitter()->sizes();
    if(sizes[3] == 0)
    {
        sizes.replace(0, sizes[0] - 60 + sizes[2]);
        sizes.replace(2, 0);
        sizes.replace(3, 60);
        FileManager::Instance.currentWidgetFile()->verticalSplitter()->widget(3)->setMaximumHeight(460);
        FileManager::Instance.currentWidgetFile()->verticalSplitter()->setSizes(sizes);
        _labelConsole->setStyleSheet(QString("background-color:")+ ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color()));
        this->closeErrorTable();
    }
    else
    {
        closeConsole();
    }
}

void WidgetStatusBar::closeConsole()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        return;
    }
    QList<int> sizes = FileManager::Instance.currentWidgetFile()->verticalSplitter()->sizes();
    sizes.replace(0, sizes[0] + sizes[3]);
    sizes.replace(3, 00);
    FileManager::Instance.currentWidgetFile()->verticalSplitter()->widget(3)->setMaximumHeight(0);
    FileManager::Instance.currentWidgetFile()->verticalSplitter()->setSizes(sizes);
    _labelConsole->setStyleSheet(QString("background-color: transparent"));
}

void WidgetStatusBar::toggleErrorTable()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        return;
    }
    QList<int> sizes = FileManager::Instance.currentWidgetFile()->verticalSplitter()->sizes();
    if(sizes[2] == 0)
    {
        sizes.replace(0, sizes[0] - 60 + sizes[3]);
        sizes.replace(2, 60);
        sizes.replace(3, 0);
        FileManager::Instance.currentWidgetFile()->verticalSplitter()->widget(2)->setMaximumHeight(460);
        FileManager::Instance.currentWidgetFile()->verticalSplitter()->setSizes(sizes);
        _labelErrorTable->setStyleSheet(QString("background-color:")+ ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color()));
        this->closeConsole();
    }
    else
    {
        this->closeErrorTable();
    }
}
void WidgetStatusBar::closeErrorTable()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        return;
    }
    QList<int> sizes = FileManager::Instance.currentWidgetFile()->verticalSplitter()->sizes();
    sizes.replace(0, sizes[0] + sizes[2]);
    sizes.replace(2, 00);
    FileManager::Instance.currentWidgetFile()->verticalSplitter()->widget(2)->setMaximumHeight(0);
    FileManager::Instance.currentWidgetFile()->verticalSplitter()->setSizes(sizes);
    _labelErrorTable->setStyleSheet(QString("background-color: transparent"));
}
void WidgetStatusBar::setPosition(int row, int column)
{
    _positionLabel->setText(trUtf8("Ligne %1, Colonne %2").arg(QString::number(row)).arg(QString::number(column)));
}

void WidgetStatusBar::setEncoding(QString encoding)
{
    _encodingLabel->setText(encoding);
}
