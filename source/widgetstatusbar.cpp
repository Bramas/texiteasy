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
#include <QBitmap>
#include <QGraphicsDropShadowEffect>

WidgetStatusBar::WidgetStatusBar(QWidget *parent) :
    QStatusBar(parent),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

    _labelConsole = new QLabel(QString("<div style='margin:5px;'><a class='link' style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Console")+"</a></div>");
    this->addPermanentWidget(_labelConsole);

    _labelErrorTable = new QLabel(QString("<div style='margin:5px;'><a style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Erreurs")+"</a></div>");


    this->addPermanentWidget(_labelErrorTable);


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







    _labelLinkSync = new WidgetStatusBarButton(this);
    QImage linkImage(":/data/img/linkSync.png");
    QImage  unlinkImage(":/data/img/unlinkSync.png");
    if(ConfigManager::Instance.darkTheme())
    {
        linkImage.invertPixels();
        unlinkImage.invertPixels();
    }
    _labelLinkSync->setCheckable(true);
    _labelLinkSync->setPixmaps(new QPixmap(QPixmap::fromImage(unlinkImage)), new QPixmap(QPixmap::fromImage(linkImage)));
    this->addPermanentWidget(_labelLinkSync);





    connect(_labelConsole, SIGNAL(linkActivated(QString)), &FileManager::Instance, SLOT(toggleConsole()));
    connect(_labelErrorTable, SIGNAL(linkActivated(QString)), &FileManager::Instance, SLOT(toggleErrorTable()));
    connect(&FileManager::Instance, SIGNAL(verticalSplitterChanged()), this, SLOT(updateButtons()));
    this->setStyleSheet("QStatusBar::item { border: none;} QStatusBar {padding:0; height:100px; background: "+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("linenumber").background().color())+
                                     "}");



    this->setMaximumHeight(20);
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}

void WidgetStatusBar::setPosition(int row, int column)
{
    _positionLabel->setText(trUtf8("Ligne %1, Colonne %2").arg(QString::number(row)).arg(QString::number(column)));
}

void WidgetStatusBar::setEncoding(QString encoding)
{
    _encodingLabel->setText(encoding);
}

void WidgetStatusBar::updateButtons()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        return;
    }
    QList<int> sizes = FileManager::Instance.currentWidgetFile()->verticalSplitter()->sizes();
    if(sizes[2] == 0)
    {
        _labelErrorTable->setStyleSheet(QString("background-color:transparent"));
    }
    else
    {
        _labelErrorTable->setStyleSheet(QString("background-color:")+ ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(200)));
    }
    if(sizes[3] == 0)
    {
        _labelConsole->setStyleSheet(QString("background-color:transparent"));
    }
    else
    {
        _labelConsole->setStyleSheet(QString("background-color:")+ ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(200)));
    }
}

void WidgetStatusBar::initTheme()
{
    this->setStyleSheet("QStatusBar::item { border: none;} QStatusBar {padding:0; height:100px; background: "+
                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("linenumber").background().color())+
                                     "}");

    _labelConsole->setText(QString("<div style='margin:5px;'><a class='link' style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Console")+"</a></div>");
    _labelErrorTable->setText(QString("<div style='margin:5px;'><a style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Erreurs")+"</a></div>");
    _positionLabel->setStyleSheet(QString("font-size:11px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));


    bool darkTheme = ConfigManager::Instance.getTextCharFormats("normal").background().color().value() < 100;
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    _labelConsole->setGraphicsEffect(effect);
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    _labelErrorTable->setGraphicsEffect(effect);
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    _positionLabel->setGraphicsEffect(effect);

    updateButtons();
}
/***************************************************
 *
 *          WidgetStatusBarButton
 *
 **************************/

void WidgetStatusBarButton::leaveEvent(QEvent *)
{
    if(_defaultPixmap)
    {
        _label->setPixmap(*_defaultPixmap);
    }
}

void WidgetStatusBarButton::enterEvent(QEvent *)
{
    if(_hoverPixmap)
    {
        _label->setPixmap(*_hoverPixmap);
    }
}
void WidgetStatusBarButton::toggleChecked()
{
    _checked = ! _checked;
    QPixmap * s = _defaultPixmap;
    _defaultPixmap = _hoverPixmap;
    _hoverPixmap = s;
    if(this->action())
    {
        this->action()->trigger();
    }
}

void WidgetStatusBarButton::toggleCheckedWithoutTriggeringAction()
{
    _checked = ! _checked;
    QPixmap * s = _defaultPixmap;
    _defaultPixmap = _hoverPixmap;
    _hoverPixmap = s;
    if(_defaultPixmap)
    {
        _label->setPixmap(*_defaultPixmap);
    }
}
void WidgetStatusBarButton::mousePressEvent(QMouseEvent *)
{
    if(this->isCheckable())
    {
        this->action()->toggle();
    }
}
void WidgetStatusBarButton::setAction(QAction *action)
{
    _action = action;
    this->setCheckable(_action->isCheckable());
    if(this->isCheckable())
    {
        this->setChecked(_action->isChecked());
        connect(_action, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
    }
}
