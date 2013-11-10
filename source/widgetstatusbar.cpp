#include "widgetstatusbar.h"
#include "ui_widgetstatusbar.h"
#include "configmanager.h"
#include "filemanager.h"
#include "minisplitter.h"

#include <QPushButton>
#include <QMenu>
#include <QDebug>
#include <QSplitter>
#include <QToolButton>
#include <QMouseEvent>
#include <QLabel>
#include <QBitmap>
#include <QGraphicsDropShadowEffect>

WidgetStatusBar::WidgetStatusBar(QWidget *parent) :
    QStatusBar(parent),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

    _labelConsole = new QLabel(QString("<div style='margin:0px;'><a class='link' style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Console")+"</a></div>");
    this->addPermanentWidget(_labelConsole);

    _labelErrorTable = new QLabel(QString("<a style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Erreurs")+"</a>");


    this->addPermanentWidget(_labelErrorTable);


    _positionLabel = new QLabel("<span>"+trUtf8("Ligne %1, Colonne %2").arg("1").arg("1")+"</span>",this);
    _positionLabel->setStyleSheet(QString("font-size:11px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_positionLabel, 0);

    QLabel* messageArea = new QLabel(this);
    messageArea->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    connect(this, SIGNAL(messageChanged(QString)), messageArea, SLOT(setText(QString)));

    this->addPermanentWidget(messageArea, 1);


    _labelDictionary = new WidgetStatusBarButton(this);
    _labelDictionary->setText(ConfigManager::Instance.currentDictionary());
    foreach(const QString dico, ConfigManager::Instance.dictionnaries())
    {
        QAction * action = new QAction(dico, _labelDictionary);
        if(!dico.compare(_labelDictionary->text()))
        {
            action->setCheckable(true);
            action->setChecked(true);
        }
        connect(action, SIGNAL(triggered()), &FileManager::Instance, SLOT(setDictionaryFromAction()));
        _labelDictionary->addAction(action);
    }
    _labelDictionary->enableLeftClickContextMenu();
    _labelDictionary->setEnabled(false);
    this->addPermanentWidget(_labelDictionary, 0);


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



    this->setMaximumHeight(25);
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}

void WidgetStatusBar::setPosition(int row, int column)
{
    _positionLabel->setText("<span>"+trUtf8("Ligne %1, Colonne %2").arg(QString::number(row)).arg(QString::number(column))+"</span>");
}

void WidgetStatusBar::setEncoding(QString encoding)
{
    _encodingLabel->setText(encoding);
}

void WidgetStatusBar::updateButtons()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        _labelDictionary->setText("");
        _labelDictionary->setEnabled(false);
        return;
    }
    // udate Console widget and errorTable widget
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
    //update dictionary label

    _labelDictionary->setEnabled(true);
    _labelDictionary->setText(FileManager::Instance.currentWidgetFile()->dictionary());
}

void WidgetStatusBar::initTheme()
{
    this->setStyleSheet("QStatusBar::item { margin-left:4px; border: none;} QStatusBar {padding:0px; height:100px; background: "+
                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("linenumber").background().color())+
                                     "}");

    _labelDictionary->label()->setStyleSheet(QString("font-size:12px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    //_labelDictionary->setStyleSheet(QString("color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));

    _labelConsole->setText(QString("<div style='font-size:12px; '><a class='link' style='text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Console")+"</a></div>");
    _labelErrorTable->setText(QString("<a style='font-size:12px; margin-top:-3px; text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Erreurs")+"</a>");

    _positionLabel->setStyleSheet(QString("font-size:12px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));


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
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    _labelDictionary->setGraphicsEffect(effect);

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
void WidgetStatusBarButton::mousePressEvent(QMouseEvent * event)
{
    if(!this->isEnabled())
    {
        return;
    }
    if(this->isCheckable() && this->action())
    {
        this->action()->toggle();
        return;
    }
    if(event->button() == Qt::RightButton || _leftClickContextMenu && event->button() == Qt::LeftButton)
    {
        if(this->actions().count())
        {
            QMenu menu(this);
            QList<QAction*> listAction = this->actions();
            menu.addActions(listAction);
            int bottom = menu.actionGeometry(listAction.last()).bottom();

            QAction * action = menu.exec(this->mapToGlobal(QPoint(0, - 6 - bottom)));
            if(action)
            {
                this->setText(action->text());
                updateGeometry();
                foreach(QAction * a, this->actions())
                {
                    a->setChecked(false);
                }
                action->setCheckable(true);
                action->setChecked(true);
            }
            return;
        }
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

void WidgetStatusBarButton::setText(QString text)
{
    _label->setText(text);
    updateGeometry();
}

void WidgetStatusBarButton::updateGeometry()
{
    QFontMetrics fm(QFont(ConfigManager::Instance.getTextCharFormats("normal").fontFamily(), 12));

    int width = fm.width(_label->text());
    _label->setMinimumWidth(width);
    _label->setMaximumWidth(width);
    this->setMinimumWidth(width);
    this->setMaximumWidth(width);
    QWidget::updateGeometry();
}
