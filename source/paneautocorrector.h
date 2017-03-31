#ifndef PANEAUTOCORRECTOR_H
#define PANEAUTOCORRECTOR_H

#include <QPlainTextEdit>
#include "ipane.h"

class Builder;
class WidgetFile;


class PaneAutoCorrector : public QObject, public IPane
{
    Q_OBJECT
public:
    explicit PaneAutoCorrector(WidgetFile * widgetFile);
    ~PaneAutoCorrector();



    QString statusbarText() { return "Auto Corrector"; }
    QWidget * paneWidget();
    QObject * getQObject();
    QAction * action() { return _action; }

signals:
    void requestLine(int);
public slots:
    void expand(void);
    void collapsed(void);
    void onError(void);
    void onSuccess(void);
    void setOutput(QString newText);

    void openMyPane();
    void closeMyPane();

protected:
    //void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);

private slots:
    void updateBuilder();

private:

    bool _collapsed;
    int _height;
    Builder * _builder;
    WidgetFile * _widgetFile;
    QAction * _action;
    QPlainTextEdit * _mainWidget;
};


#endif // PANEAUTOCORRECTOR_H
