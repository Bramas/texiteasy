#ifndef QT5PANECALLBACK_H
#define QT5PANECALLBACK_H

#include <QObject>

//#if QT_VERSION < QT_VERSION_CHECK(5,0,0)


class QAction;
class IPane;
class OutputPaneToggleButton;
class WidgetFile;

class Qt5PaneCallback : public QObject
{
    Q_OBJECT
public:
    explicit Qt5PaneCallback(QObject *parent = 0);


    OutputPaneToggleButton *button;
    IPane * pane;
    WidgetFile * widget;

signals:

public slots:
    void onButtonClicked(bool);
    void onPaneactionToggled(bool checked);
};

//#endif

#endif // QT5PANECALLBACK_H
