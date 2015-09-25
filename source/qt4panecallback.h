#ifndef QT4PANECALLBACK_H
#define QT4PANECALLBACK_H

#include <QObject>

//#if QT_VERSION < QT_VERSION_CHECK(5,0,0)


class QAction;
class IPane;
class OutputPaneToggleButton;
class WidgetFile;

class Qt4PaneCallback : public QObject
{
    Q_OBJECT
public:
    explicit Qt4PaneCallback(QObject *parent = 0);


    OutputPaneToggleButton *button;
    IPane * pane;
    WidgetFile * widget;

signals:

public slots:
    void onButtonClicked(bool);
    void onPaneactionToggled(bool checked);
};

//#endif

#endif // QT4PANECALLBACK_H
