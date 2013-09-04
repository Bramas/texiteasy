#ifndef WIDGETSTATUSBAR_H
#define WIDGETSTATUSBAR_H

#include <QStatusBar>

namespace Ui {
class WidgetStatusBar;
}
class QPushButton;
class QSplitter;

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
    
private:
    Ui::WidgetStatusBar *ui;
    QPushButton * _pushButtonConsole;
    QPushButton * _pushButtonErreurs;
    QSplitter * _leftSplitter;
};

#endif // WIDGETSTATUSBAR_H
