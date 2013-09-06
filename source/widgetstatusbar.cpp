#include "widgetstatusbar.h"
#include "ui_widgetstatusbar.h"
#include "configmanager.h"

#include <QPushButton>
#include <QDebug>
#include <QSplitter>
#include <QToolButton>
#include <QLabel>

WidgetStatusBar::WidgetStatusBar(QWidget *parent, QSplitter * leftSplitter) :
    QStatusBar(parent),
    _leftSplitter(leftSplitter),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

    QList<int> sizes = _leftSplitter->sizes();

    _pushButtonConsole = new QToolButton();
    _pushButtonConsole->setText(trUtf8("Console"));
    _pushButtonConsole->setCheckable(true);
    _pushButtonConsole->setChecked(sizes[3] != 0);
    _pushButtonConsole->setAutoRaise(true);
    _pushButtonConsole->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_pushButtonConsole, 0);


    _pushButtonErreurs = new QToolButton();
    _pushButtonErreurs->setText(trUtf8("Erreurs"));
    _pushButtonErreurs->setCheckable(true);
    _pushButtonErreurs->setChecked(sizes[2] != 0);
    _pushButtonErreurs->setAutoRaise(true);
    _pushButtonErreurs->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_pushButtonErreurs, 0);

    _positionLabel = new QLabel(trUtf8("Ligne %1, Colonne %2").arg("1").arg("1"),this);
    _positionLabel->setStyleSheet(QString("font-size:11px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_positionLabel, 0);

    QLabel* messageArea = new QLabel(this);
    messageArea->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    connect(this, SIGNAL(messageChanged(QString)), messageArea, SLOT(setText(QString)));
    this->addPermanentWidget(messageArea, 1);


    _encodingLabel = new QLabel(this);
    _encodingLabel->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_encodingLabel, 0);

    connect(_leftSplitter, SIGNAL(splitterMoved(int,int)), this, SLOT(onLeftSplitterMoved(int,int)));
    connect(_pushButtonConsole, SIGNAL(clicked()), this, SLOT(toggleConsole()));
    connect(_pushButtonErreurs, SIGNAL(clicked()), this, SLOT(toggleErrorTable()));

    this->setStyleSheet("QStatusBar::item { border: none;} QStatusBar {padding:0; height:100px; background: "+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color())+
                                     "}");

    this->setMaximumHeight(20);
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}


void WidgetStatusBar::onLeftSplitterMoved(int pos, int index)
{
    QList<int> sizes = _leftSplitter->sizes();
    _pushButtonErreurs->setChecked(sizes[2] != 0);
    _pushButtonConsole->setChecked(sizes[3] != 0);
}

void WidgetStatusBar::toggleConsole()
{
    QList<int> sizes = _leftSplitter->sizes();
    if(sizes[3] == 0)
    {
        sizes.replace(0, sizes[0] - 60);
        sizes.replace(3, 60);
    }
    else
    {
        sizes.replace(0, sizes[0] + sizes[3]);
        sizes.replace(3, 00);
    }
    _leftSplitter->setSizes(sizes);
}

void WidgetStatusBar::toggleErrorTable()
{
    QList<int> sizes = _leftSplitter->sizes();
    if(sizes[2] == 0)
    {
        sizes.replace(0, sizes[0] - 60);
        sizes.replace(2, 60);
    }
    else
    {
        sizes.replace(0, sizes[0] + sizes[2]);
        sizes.replace(2, 00);
    }
    _leftSplitter->setSizes(sizes);
}
void WidgetStatusBar::setPosition(int row, int column)
{
    _positionLabel->setText(trUtf8("Ligne %1, Colonne %2").arg(QString::number(row)).arg(QString::number(column)));
}

void WidgetStatusBar::setEncoding(QString encoding)
{
    _encodingLabel->setText(encoding);
}
