#ifndef WIDGETSIMPLEOUTPUT_H
#define WIDGETSIMPLEOUTPUT_H

#include <QTableWidget>

class Builder;
class WidgetTextEdit;

class WidgetSimpleOutput : public QTableWidget
{
    Q_OBJECT
public:
    explicit WidgetSimpleOutput(QWidget *parent = 0);
    void setBuilder(Builder * builder);
    void setWidgetTextEdit(WidgetTextEdit * widgetTextEdit) { _widgetTextEdit = widgetTextEdit; }
    
signals:
    
public slots:
    void onError(void);
    void onSuccess(void);
    void onCellSelected(int,int);
    
private:
    Builder * _builder;
    WidgetTextEdit * _widgetTextEdit;
};

#endif // WIDGETSIMPLEOUTPUT_H
