#ifndef WIDGETFINDREPLACE_H
#define WIDGETFINDREPLACE_H

#include <QWidget>

class QPushButton;
class WidgetTextEdit;
namespace Ui {
class WidgetFindReplace;
}

class WidgetFindReplace : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetFindReplace(WidgetTextEdit * parent = 0);
    ~WidgetFindReplace();

    QPushButton * pushButtonClose();
    void open();
    void initTheme();

public slots:
    bool find(int from = -1, bool canStartOver = true, bool backward = false);
    bool findBackward(int from = -1, bool canStartOver = true);
    void replace();
    bool replaceAndFind();
    void replaceAll();

protected:
    void changeEvent(QEvent *event);
private:
    Ui::WidgetFindReplace *ui;

    WidgetTextEdit * _widgetTextEdit;
};

#endif // WIDGETFINDREPLACE_H
