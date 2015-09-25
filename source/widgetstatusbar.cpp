#include "widgetstatusbar.h"
#include "ui_widgetstatusbar.h"
#include "configmanager.h"
#include "filemanager.h"
#include "minisplitter.h"
#include "widgetfile.h"
#include "filestructure.h"
#include "widgettextedit.h"
#include "taskpane/taskwindow.h"
#include "ipane.h"

#include <QPushButton>
#include <QMenu>
#include <QDebug>
#include <QSplitter>
#include <QToolButton>
#include <QMouseEvent>
#include <QLabel>
#include <QBitmap>
#include <QGraphicsDropShadowEffect>
#include <QTimer>



WidgetStatusBar::WidgetStatusBar(QWidget *parent) :
    QStatusBar(parent),
    ui(new Ui::WidgetStatusBar)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::PreventContextMenu);




    {
    _labelSplitEditor = new WidgetStatusBarButton(this);
    QImage incorporatedImage(":/data/img/splitbutton_horizontal.png");
    QImage  separatedImage(":/data/img/splitbutton_closetop.png");
    QImage incorporatedHoverImage(":/data/img/splitbutton_horizontal.png");
    QImage  separatedHoverImage(":/data/img/splitbutton_closetop.png");
    if(!ConfigManager::Instance.darkTheme())
    {
        incorporatedImage.invertPixels();
        separatedImage.invertPixels();
        incorporatedHoverImage.invertPixels();
        separatedHoverImage.invertPixels();
    }
    _labelSplitEditor->setCheckable(true);
    _labelSplitEditor->setPixmaps(new QPixmap(QPixmap::fromImage(incorporatedImage)),
                                              new QPixmap(QPixmap::fromImage(separatedImage)),
                                              new QPixmap(QPixmap::fromImage(incorporatedHoverImage)),
                                              new QPixmap(QPixmap::fromImage(separatedHoverImage))
                                              );
    this->addPermanentWidget(_labelSplitEditor);
    }


    _positionLabel = new QLabel("<span>"+trUtf8("Ligne %1, Colonne %2").arg("1").arg("1")+"</span>",this);
    _positionLabel->setStyleSheet(QString("font-size:11px; "
                                          "margin-right:5px; "
                                          "color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    this->addPermanentWidget(_positionLabel, 0);

    QLabel* messageArea = new QLabel(this);
    messageArea->setStyleSheet(QString("font-size:11px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    connect(this, SIGNAL(messageChanged(QString)), messageArea, SLOT(setText(QString)));

    this->addPermanentWidget(messageArea, 1);

    //Structur
    _labelStruct = new WidgetStatusBarButton(this);
    _labelStruct->setText("Document");
    _labelStruct->enableLeftClickContextMenu();
    _labelStruct->setEnabled(false);
    this->addPermanentWidget(_labelStruct, 0);


    // Dictionnary
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




    _labelPdfViewerInItsOwnWidget = new WidgetStatusBarButton(this);
    QImage incorporatedImage(":/data/img/incorporatedPdf.png");
    QImage  separatedImage(":/data/img/separatedPdf.png");
    QImage incorporatedHoverImage(":/data/img/incorporatedPdfHover.png");
    QImage  separatedHoverImage(":/data/img/separatedPdfHover.png");
    if(ConfigManager::Instance.darkTheme())
    {
        incorporatedImage.invertPixels();
        separatedImage.invertPixels();
        incorporatedHoverImage.invertPixels();
        separatedHoverImage.invertPixels();
    }
    _labelPdfViewerInItsOwnWidget->setCheckable(true);
    _labelPdfViewerInItsOwnWidget->setPixmaps(new QPixmap(QPixmap::fromImage(incorporatedImage)),
                                              new QPixmap(QPixmap::fromImage(separatedImage)),
                                              new QPixmap(QPixmap::fromImage(incorporatedHoverImage)),
                                              new QPixmap(QPixmap::fromImage(separatedHoverImage))
                                              );
    this->addPermanentWidget(_labelPdfViewerInItsOwnWidget);


    //connect(_labelConsole, SIGNAL(linkActivated(QString)), &FileManager::Instance, SLOT(toggleConsole()));
    //connect(_labelErrorTable, SIGNAL(linkActivated(QString)), &FileManager::Instance, SLOT(toggleErrorTable()));
    //connect(_labelWarningPane, SIGNAL(linkActivated(QString)), &FileManager::Instance, SLOT(toggleWarningPane()));
    //connect(&FileManager::Instance, SIGNAL(verticalSplitterChanged()), this, SLOT(updateButtons()));



    //this->setMaximumHeight(25);
    this->setContentsMargins(0,0,0,0);
}

WidgetStatusBar::~WidgetStatusBar()
{
    delete ui;
}

void WidgetStatusBar::cursorPositionChanged(int row, int column)
{
    setPosition(row, column);
    updateStruct();
}

void WidgetStatusBar::setPosition(int row, int column)
{
    _positionLabel->setText("<span>"+trUtf8("Ligne %1, Colonne %2").arg(QString::number(row)).arg(QString::number(column))+"</span>");
}

void WidgetStatusBar::setEncoding(QString encoding)
{
    _encodingLabel->setText(encoding);
}

void WidgetStatusBar::updateStruct()
{
    _labelStruct->removeActions();
    WidgetFile * widget = FileManager::Instance.currentWidgetFile();

    //Stop if there is no opened file
    if(!widget)
    {
        return;
    }
    QStringList structure = widget->widgetTextEdit()->textStruct()->sectionsList("  ");
    QString currentSection = widget->widgetTextEdit()->textStruct()->currentSection();
    if(structure.isEmpty() || currentSection.isEmpty())
    {
        _labelStruct->setText("Document");
    }
    else
    {
        _labelStruct->setText(currentSection);
    }
    foreach(const QString line, structure)
    {
        QAction * action = new QAction(line, _labelStruct);
        connect(action, SIGNAL(triggered()), &FileManager::Instance, SLOT(goToSection()));
        _labelStruct->addAction(action);
    }
    QTimer::singleShot(1, this, SLOT(checkStructAction())); // do it after because sometimes it does not work
}
void WidgetStatusBar::checkStructAction()
{
    foreach(QAction * action, _labelStruct->actions())
    {
        if(!_labelStruct->text().trimmed().compare(action->text().trimmed()))
        {
            action->setCheckable(true);
            action->setChecked(true);
        }
    }
}






void WidgetStatusBar::updateButtons()
{
    if(!FileManager::Instance.currentWidgetFile())
    {
        _labelDictionary->setText("");
        _labelDictionary->setEnabled(false);
        return;
    }

    foreach(OutputPaneToggleButton *b, _paneLabels)
    {
        removeWidget(b);
        delete b;
    }
    _paneLabels.clear();

    WidgetFile * widget = FileManager::Instance.currentWidgetFile();
    int index = 1;
    foreach(IPane * pane, widget->panes())
    {


        OutputPaneToggleButton *button = new OutputPaneToggleButton(index, pane->statusbarText(),
                                                                    pane->action());

        if(widget->isPaneOpen(pane))
        {
            button->setChecked(true);
        }

        pane->action()->disconnect();
        this->insertPermanentWidget(index, button, 0);
        _paneLabels << button;

#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
        Qt4PaneCallback * c = new Qt4PaneCallback();
        c->pane = pane;
        c->widget = widget;
        c->button = button;

        connect(button, SIGNAL(clicked(bool)), c, SLOT(onButtonClicked(bool)));
        connect(pane->action(), SIGNAL(toggled(bool)), c, SLOT(onPaneactionToggled(bool)));
        connect(pane->action(), SIGNAL(triggered(bool)), c, SLOT(onPaneactionToggled(bool)));

#else
        connect(button, &OutputPaneToggleButton::clicked, [=](){
            widget->togglePane(pane);
        });
        connect(pane->action(), &QAction::toggled, [=](){ button->setChecked(pane->action()->isChecked()); });
        connect(pane->action(), &QAction::triggered, [=](bool checked){ button->setChecked(checked); });
#endif
        ++index;
    }

    //update struct label
    updateStruct();
    _labelStruct->setEnabled(true);


    updateTaskPane();

    //update dictionary label
    _labelDictionary->setEnabled(true);
    _labelDictionary->setText(FileManager::Instance.currentWidgetFile()->dictionary());

    //update Encoding label
    _encodingLabel->setText(FileManager::Instance.currentWidgetFile()->file()->codec());


}

void WidgetStatusBar::updateTaskPane()
{
   /* WidgetFile * w = FileManager::Instance.currentWidgetFile();
    QString warnings;
    if(w && w->taskPane()->warningTaskCount())
    {
        warnings = " "+QString::number(w->taskPane()->warningTaskCount())+" <img src=\"qrc:/data/img/warning.png\" />";
    }
    _labelWarningPane->setText(QString("<a style='font-size:12px; margin-top:-3px; text-decoration:none; color:")+
            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+ "' href='#'>"+trUtf8("Warning")+warnings+"</a>"
                );
    _labelErrorTable->setText(QString("<a style='font-size:12px; margin-top:-3px; text-decoration:none; color:")+
                                ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+
                               "' href='#'>"+trUtf8("Erreurs")+"</a>");
                               */

}

void WidgetStatusBar::initTheme()
{
    this->setStyleSheet("QStatusBar::item { margin-left:4px; border: none;} QStatusBar {padding:0px; height:100px; background: "+
                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("line-number").background().color())+
                                     "}");

    _labelStruct->label()->setStyleSheet(QString("font-size:12px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));

    _labelDictionary->label()->setStyleSheet(QString("font-size:12px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    //_labelDictionary->setStyleSheet(QString("color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));


    updateTaskPane();
    _positionLabel->setStyleSheet(QString("font-size:12px; margin-right:5px; color:")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));


    bool darkTheme = ConfigManager::Instance.getTextCharFormats("normal").background().color().value() < 100;
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    //_labelConsole->setGraphicsEffect(effect);
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    //_labelErrorTable->setGraphicsEffect(effect);
    effect= new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(0);
    effect->setColor(ConfigManager::Instance.getTextCharFormats("normal").background().color().darker(darkTheme ? 400 : 130));
    effect->setOffset(darkTheme ? -1 : 1, darkTheme ? -1 : 1);
    //_labelWarningPane->setGraphicsEffect(effect);
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
    if(isChecked())
    {
        if(_checkedPixmap)
        {
            _label->setPixmap(*_checkedPixmap);
        }
    }
    else
    {
        if(_defaultPixmap)
        {
            _label->setPixmap(*_defaultPixmap);
        }
    }
}

void WidgetStatusBarButton::enterEvent(QEvent *)
{
    if(isChecked())
    {
        if(_checkedHoverPixmap)
        {
            _label->setPixmap(*_checkedHoverPixmap);
        }
    }
    else
    {
        if(_defaultHoverPixmap)
        {
            _label->setPixmap(*_defaultHoverPixmap);
        }
    }
}
void WidgetStatusBarButton::setChecked(bool checked)
{
    if(this->action() && checked != this->action()->isChecked())
    {
        this->action()->toggle();
        return;
    }
    if(_checked != checked)
    {
        this->toggleCheckedWithoutTriggeringAction();
    }
}

void WidgetStatusBarButton::toggleChecked()
{
    _checked = ! _checked;
    if(this->action())
    {
        this->action()->trigger();
    }
}

void WidgetStatusBarButton::toggleCheckedWithoutTriggeringAction()
{
    _checked = ! _checked;
    if(isChecked())
    {
        if(_checkedPixmap)
        {
            _label->setPixmap(*_checkedPixmap);
        }
    }
    else
    {
        if(_defaultPixmap)
        {
            _label->setPixmap(*_defaultPixmap);
        }
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
    if(event->button() == Qt::RightButton || (_leftClickContextMenu && event->button() == Qt::LeftButton))
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
    QFontMetrics fm(QFont(this->_label->font().family(), 12));

    int width = fm.width(_label->text()) + 10;
    _label->setMinimumWidth(width);
    _label->setMaximumWidth(width);
    this->setMinimumWidth(width);
    this->setMaximumWidth(width);
    QWidget::updateGeometry();
}

void WidgetStatusBarButton::removeActions()
{
    foreach(QAction * action, this->actions())
    {
        removeAction(action);
    }
}



///////////////////////////////////////////////////////////////////////
//
// OutputPaneToolButton
//
///////////////////////////////////////////////////////////////////////

static char outputPaneSettingsKeyC[] = "OutputPaneVisibility";
static char outputPaneIdKeyC[] = "id";
static char outputPaneVisibleKeyC[] = "visible";
static const int numberAreaWidth = 19;
static const int buttonBorderWidth = 10;


OutputPaneToggleButton::OutputPaneToggleButton(int number, const QString &text,
                                               QAction *action, QWidget *parent)
    : QToolButton(parent)
    , m_number(QString::number(number))
    , m_text(text)
    , m_action(action)
    , m_flashTimer(new QTimeLine(1000, this))
{
    setFocusPolicy(Qt::NoFocus);
    setCheckable(true);
    QFont fnt = QApplication::font();
    setFont(fnt);
    if (m_action)
        connect(m_action, SIGNAL(changed()), this, SLOT(updateToolTip()));

    m_flashTimer->setDirection(QTimeLine::Forward);
    m_flashTimer->setCurveShape(QTimeLine::SineCurve);
    m_flashTimer->setFrameRange(0, 92);
    connect(m_flashTimer, SIGNAL(valueChanged(qreal)), this, SLOT(update()));
    connect(m_flashTimer, SIGNAL(finished()), this, SLOT(update()));
    this->setMinimumHeight(25);
}

void OutputPaneToggleButton::updateToolTip()
{
    Q_ASSERT(m_action);
    setToolTip(m_action->toolTip());
}

QSize OutputPaneToggleButton::sizeHint() const
{
    ensurePolished();

    QSize s = fontMetrics().size(Qt::TextSingleLine, m_text);

    // Expand to account for border image
    s.rwidth() += 1 + buttonBorderWidth + buttonBorderWidth;// + numberAreaWidth;

    //if (!m_badgeNumberLabel.text().isNull())
    //    s.rwidth() += m_badgeNumberLabel.sizeHint().width() + 1;

    return s.expandedTo(QApplication::globalStrut());
}


QString dpiSpecificImageFile(const QString &fileName)
{
    // See QIcon::addFile()
    qreal ratio = 1.0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    ratio = qApp->devicePixelRatio(); // 2 if retina, 1 otherwise
#endif
    if (ratio > 1.0) {
        const QFileInfo fi(fileName);
        const QString at2xfileName = fi.path() + QLatin1Char('/')
                + fi.completeBaseName() + QStringLiteral("@2x.") + fi.suffix();
        if (QFile::exists(at2xfileName))
            return at2xfileName;
    }
    return fileName;
}


// Draws a CSS-like border image where the defined borders are not stretched
// Unit for rect, left, top, right and bottom is user pixels
void drawCornerImage(const QImage &img, QPainter *painter, const QRect &rect,
                                  int left, int top, int right, int bottom)
{
    // source rect for drawImage() calls needs to be specified in DIP unit of the image
    const qreal imagePixelRatio = 1.0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    imagePixelRatio = img.devicePixelRatio(); // 2 if retina, 1 otherwise
#endif
    const qreal leftDIP = left * imagePixelRatio;
    const qreal topDIP = top * imagePixelRatio;
    const qreal rightDIP = right * imagePixelRatio;
    const qreal bottomDIP = bottom * imagePixelRatio;

    const QSize size = img.size();
    if (top > 0) { //top
        painter->drawImage(QRectF(rect.left() + left, rect.top(), rect.width() -right - left, top), img,
                           QRectF(leftDIP, 0, size.width() - rightDIP - leftDIP, topDIP));
        if (left > 0) //top-left
            painter->drawImage(QRectF(rect.left(), rect.top(), left, top), img,
                               QRectF(0, 0, leftDIP, topDIP));
        if (right > 0) //top-right
            painter->drawImage(QRectF(rect.left() + rect.width() - right, rect.top(), right, top), img,
                               QRectF(size.width() - rightDIP, 0, rightDIP, topDIP));
    }
    //left
    if (left > 0)
        painter->drawImage(QRectF(rect.left(), rect.top()+top, left, rect.height() - top - bottom), img,
                           QRectF(0, topDIP, leftDIP, size.height() - bottomDIP - topDIP));
    //center
    painter->drawImage(QRectF(rect.left() + left, rect.top()+top, rect.width() -right - left,
                              rect.height() - bottom - top), img,
                       QRectF(leftDIP, topDIP, size.width() - rightDIP - leftDIP,
                              size.height() - bottomDIP - topDIP));
    if (right > 0) //right
        painter->drawImage(QRectF(rect.left() +rect.width() - right, rect.top()+top, right, rect.height() - top - bottom), img,
                           QRectF(size.width() - rightDIP, topDIP, rightDIP, size.height() - bottomDIP - topDIP));
    if (bottom > 0) { //bottom
        painter->drawImage(QRectF(rect.left() +left, rect.top() + rect.height() - bottom,
                                  rect.width() - right - left, bottom), img,
                           QRectF(leftDIP, size.height() - bottomDIP,
                                  size.width() - rightDIP - leftDIP, bottomDIP));
        if (left > 0) //bottom-left
            painter->drawImage(QRectF(rect.left(), rect.top() + rect.height() - bottom, left, bottom), img,
                               QRectF(0, size.height() - bottomDIP, leftDIP, bottomDIP));
        if (right > 0) //bottom-right
            painter->drawImage(QRectF(rect.left() + rect.width() - right, rect.top() + rect.height() - bottom, right, bottom), img,
                               QRectF(size.width() - rightDIP, size.height() - bottomDIP, rightDIP, bottomDIP));
    }
}
void OutputPaneToggleButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    const QFontMetrics fm = fontMetrics();
    const int baseLine = (height() - fm.height() + 1) / 2 + fm.ascent();
    /*
    static const QImage panelButton(dpiSpecificImageFile(QStringLiteral(":/data/img/panel_button.png")));
    static const QImage panelButtonHover(dpiSpecificImageFile(QStringLiteral(":/data/img/panel_button_hover.png")));
    static const QImage panelButtonPressed(dpiSpecificImageFile(QStringLiteral(":/data/img/panel_button_pressed.png")));
    static const QImage panelButtonChecked(dpiSpecificImageFile(QStringLiteral(":/data/img/panel_button_checked.png")));
    static const QImage panelButtonCheckedHover(dpiSpecificImageFile(QStringLiteral(":/data/img/panel_button_checked_hover.png")));

    const int numberWidth = fm.width(m_number);


    QStyleOption styleOption;
    styleOption.initFrom(this);
#ifdef OS_MAC
    const bool hovered = false;
#else
    const bool hovered = (styleOption.state & QStyle::State_MouseOver);
#endif

    const QImage *image = 0;
    //if (creatorTheme()->widgetStyle() == Theme::StyleDefault)
    {
        if (isDown())
            image = &panelButtonPressed;
        else if (isChecked())
            image = hovered ? &panelButtonCheckedHover : &panelButtonChecked;
        else
            image = hovered ? &panelButtonHover : &panelButton;
        if (image)
            drawCornerImage(*image, &p, rect(), numberAreaWidth, buttonBorderWidth, buttonBorderWidth, buttonBorderWidth);
    }*/

    if(isChecked())
    {
        p.save();
        p.setBrush(QBrush(ConfigManager::Instance.getTextCharFormats("selected-line").background().color()));
        p.setPen(QPen(ConfigManager::Instance.getTextCharFormats("selected-line").background().color().darker(50)));
        QRect r = rect();
        r.setBottomRight(r.bottomRight() - QPoint(1,1));
        p.drawRect(r);
        p.restore();
    }

    p.setFont(font());
    p.setPen(QPen(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    //p.drawText((numberAreaWidth - numberWidth) / 2, baseLine, m_number);
    if (!isChecked())
        p.setPen(QPen(ConfigManager::Instance.getTextCharFormats("normal").foreground().color()));
    int leftPart = buttonBorderWidth; //numberAreaWidth + buttonBorderWidth;
    int labelWidth = 0;
    /*if (!m_badgeNumberLabel.text().isEmpty()) {
        const QSize labelSize = m_badgeNumberLabel.sizeHint();
        labelWidth = labelSize.width() + 3;
        m_badgeNumberLabel.paint(&p, width() - labelWidth, (height() - labelSize.height()) / 2, isChecked());
    }*/
    p.drawText(leftPart, baseLine, fm.elidedText(m_text, Qt::ElideRight, width() - leftPart - 1 - labelWidth));
}

void OutputPaneToggleButton::checkStateSet()
{
    //Stop flashing when button is checked
    QToolButton::checkStateSet();
    m_flashTimer->stop();
}

void OutputPaneToggleButton::flash(int count)
{
    setVisible(true);
    //Start flashing if button is not checked
    if (!isChecked()) {
        m_flashTimer->setLoopCount(count);
        if (m_flashTimer->state() != QTimeLine::Running)
            m_flashTimer->start();
        update();
    }
}

void OutputPaneToggleButton::setIconBadgeNumber(int number)
{
    QString text = (number ? QString::number(number) : QString());
    m_badgeNumberLabel.setText(text);
    updateGeometry();
}


BadgeLabel::BadgeLabel()
{
    m_font = QApplication::font();
    m_font.setBold(true);
    m_font.setPixelSize(11);
}

void BadgeLabel::paint(QPainter *p, int x, int y, bool isChecked)
{
    const QRectF rect(QRect(QPoint(x, y), m_size));
    p->save();

    p->setBrush(QBrush(QColor(50,50,50)));
    p->setPen(Qt::NoPen);
    p->setRenderHint(QPainter::Antialiasing, true);
    p->drawRoundedRect(rect, m_padding, m_padding, Qt::AbsoluteSize);

    p->setFont(m_font);
    p->setPen(QPen(QColor(150,150,150)));
    p->drawText(rect, Qt::AlignCenter, m_text);

    p->restore();
}

void BadgeLabel::setText(const QString &text)
{
    m_text = text;
    calculateSize();
}

QString BadgeLabel::text() const
{
    return m_text;
}

QSize BadgeLabel::sizeHint() const
{
    return m_size;
}

void BadgeLabel::calculateSize()
{
    const QFontMetrics fm(m_font);
    m_size = fm.size(Qt::TextSingleLine, m_text);
    m_size.setWidth(m_size.width() + m_padding * 1.5);
    m_size.setHeight(2 * m_padding + 1); // Needs to be uneven for pixel perfect vertical centering in the button
}

