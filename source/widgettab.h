#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QWidget>
#include <QList>
#include <QStringList>
class WidgetFile;

class WidgetTab : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetTab(QWidget *parent = 0);

    void addTab(WidgetFile* widget, QString name)
    {
        _tabsName.append(name);
        _widgets.append(widget);
        if(this->currentIndex() == -1)
        {
            this->setCurrentIndex(0);
        }
        update();
    }
    WidgetFile * widget(int index) { return _widgets.at(index); }
    void removeTab(WidgetFile * widget);
    void removeTab(int index);
    void setCurrentIndex(int index)
    {
        _currentIndex =  index;
        if(index != -1)
        {
            emit currentChanged(_widgets.at(index));
        }
        else
        {
            emit currentChanged(0);
        }
        update();
    }
    void setTabText(int index, QString name) { _tabsName.replace(index, name); update(); }
    int currentIndex() const { return _currentIndex; }
    int count() const { return _tabsName.count(); }
    
signals:
    void currentChanged(WidgetFile*);
    void tabCloseRequested(int index);
public slots:

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    
private:

    bool overCloseButton(QPoint mousePos, int left);

    QStringList _tabsName;
    QList<WidgetFile *> _widgets;
    QList<int> _tabsNameWidth;
    int _currentIndex;

    int _overCloseId;
    int _padding;
    int _margin;
    int _closeLeftMargin ;
    int _closeWidth;
};

#endif // WIDGETTAB_H
