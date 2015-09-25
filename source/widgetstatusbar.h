#ifndef WIDGETSTATUSBAR_H
#define WIDGETSTATUSBAR_H

#include <QStatusBar>
#include <QLabel>
#include <QToolButton>
#include <QTimeLine>
#include <QDebug>

#include "widgetfile.h"

namespace Ui {
class WidgetStatusBar;
}
class QToolButton;
class QSplitter;
class QPixmap;
class QAction;
class IPane;





class WidgetStatusBarButton : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetStatusBarButton(QWidget * parent) :
        QWidget(parent),
        _action(0),
        _checked(false),
        _checkedHoverPixmap(0),
        _checkedPixmap(0),
        _defaultHoverPixmap(0),
        _defaultPixmap(0),
        _label(new QLabel(this))
    {
        _checkable = false;
        _leftClickContextMenu = false;
        this->setCursor(Qt::PointingHandCursor);
        this->setMinimumSize(20,20);
        this->setContextMenuPolicy(Qt::NoContextMenu);
    }
    void      setAction(QAction * action);
    QAction * action() { return _action; }
    void removeActions();

    void setPixmaps(QPixmap * defaultPixmap, QPixmap * checkedPixmap, QPixmap * defaultHoverPixmap = 0, QPixmap * checkedHoverPixmap = 0)
    {
        _defaultPixmap = defaultPixmap;
        _checkedPixmap   = checkedPixmap;
        _defaultHoverPixmap = defaultHoverPixmap ? defaultHoverPixmap : checkedPixmap;
        _checkedHoverPixmap = checkedHoverPixmap ? checkedHoverPixmap : defaultPixmap;
        if(_defaultPixmap)
        {
            _label->setPixmap(*_defaultPixmap);
        }
    }
    void setText(QString text);
    QString text() { return _label->text(); }
    void enableLeftClickContextMenu(bool enable = true) { _leftClickContextMenu = enable; }
    void setCheckable(bool checkable) { _checkable = checkable; }
    bool isCheckable() { return _checkable; }
    bool isChecked() { return _checked; }

    void updateGeometry();
    QLabel * label() { return _label; }

public slots:
    void toggleChecked();
    void setChecked(bool checked);
private slots:
    void toggleCheckedWithoutTriggeringAction();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);


QAction * _action;

bool _leftClickContextMenu;
bool _checkable;
bool _checked;

QPixmap * _checkedHoverPixmap;
QPixmap * _checkedPixmap;
QPixmap * _defaultHoverPixmap;
QPixmap * _defaultPixmap;
QLabel  * _label;


};


class BadgeLabel
{
public:
    BadgeLabel();
    void paint(QPainter *p, int x, int y, bool isChecked);
    void setText(const QString &text);
    QString text() const;
    QSize sizeHint() const;

private:
    void calculateSize();

    QSize m_size;
    QString m_text;
    QFont m_font;
    static const int m_padding = 6;
};

class OutputPaneToggleButton : public QToolButton
{
    Q_OBJECT
public:
    OutputPaneToggleButton(int number, const QString &text, QAction *action,
                           QWidget *parent = 0);
    QSize sizeHint() const;
    void paintEvent(QPaintEvent*);
    void flash(int count = 3);
    void setIconBadgeNumber(int number);

private slots:
    void updateToolTip();

private:
    void checkStateSet();

    QString m_number;
    QString m_text;
    QAction *m_action;
    QTimeLine *m_flashTimer;
    BadgeLabel m_badgeNumberLabel;
};

class WidgetStatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit WidgetStatusBar(QWidget *parent);
    ~WidgetStatusBar();
    void initTheme();

public slots:
    void cursorPositionChanged(int row, int column);
    void setPosition(int,int);
    void setEncoding(QString encoding);

    void updateButtons();
    void setLinkSyncAction(QAction * action) { _labelLinkSync->setAction(action); }
    void setPdfViewerWidgetAction(QAction * action) { _labelPdfViewerInItsOwnWidget->setAction(action); }
    void setSplitEditorAction(QAction * action) { _labelSplitEditor->setAction(action); }
    void showTemporaryMessage(QString message) { this->showMessage(message, 4000); }
private slots:
    void checkStructAction();
private:
    void updateTaskPane();
    void updateStruct();
    Ui::WidgetStatusBar *ui;
    QToolButton * _pushButtonConsole;
    QToolButton * _pushButtonErreurs;
    QLabel * _positionLabel;
    QLabel * _encodingLabel;
    WidgetStatusBarButton * _labelLinkSync;
    WidgetStatusBarButton * _labelPdfViewerInItsOwnWidget;
    WidgetStatusBarButton * _labelDictionary;
    WidgetStatusBarButton * _labelStruct;
    WidgetStatusBarButton * _labelSplitEditor;

    QList<OutputPaneToggleButton*> _paneLabels;
    bool _errorTableOpen, _consoleOpen;
};






#endif // WIDGETSTATUSBAR_H
