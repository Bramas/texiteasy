#ifndef WIDGETSTATUSBAR_H
#define WIDGETSTATUSBAR_H

#include <QStatusBar>

namespace Ui {
class WidgetStatusBar;
}
class QToolButton;
class QSplitter;
class QLabel;

class WidgetStatusBar : public QStatusBar
{
    Q_OBJECT
    
public:
    explicit WidgetStatusBar(QWidget *parent, QSplitter *leftSplitter);
    ~WidgetStatusBar();

public slots:
    void onLeftSplitterMoved(int pos, int index);
    void toggleConsole();
    void toggleErrorTable();
    void setPosition(int,int);
    void setEncoding(QString encoding);
    
private:
    Ui::WidgetStatusBar *ui;
    QToolButton * _pushButtonConsole;
    QToolButton * _pushButtonErreurs;
    QLabel * _positionLabel;
    QLabel * _encodingLabel;
    QSplitter * _leftSplitter;
};

#endif // WIDGETSTATUSBAR_H
