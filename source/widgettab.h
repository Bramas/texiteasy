#ifndef WIDGETTAB_H
#define WIDGETTAB_H

#include <QWidget>
#include <QList>
#include <QStringList>
#include <QFont>
class WidgetFile;
class QPainter;

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
    /**
     * @brief widget
     * @param filename
     * @return the widgetFile associated with the filename. return -1 if filename is not open.
     */
    WidgetFile * widget(QString filename);
    /**
     * @brief indexOf
     * @param filename
     * @return the index of the widgetFile associated with the filename. return -1 if filename is not open.
     */
    int indexOf(QString filename);

    void removeTab(WidgetFile * widget);
    void removeTab(int index);
    void removeAll();

    void setCurrentIndex(int index);
    void initTheme();
    int currentIndex() const { return _currentIndex; }
    int count() const { return _tabsName.count(); }
signals:
    void currentChanged(WidgetFile*);
    void tabCloseRequested(int index);
    void newTabRequested();
public slots:
    void setTabText(int index, QString name)
    {
        if(index >= _tabsName.count())
            return;
        _tabsName.replace(index, name);
        update();
    }
    void setTabText(WidgetFile* tab, QString name)
    {
        name.replace(QRegExp("^.*[\\\\\\/]([^\\\\\\/]*)$"),"\\1");
        int index = _widgets.indexOf(tab);
        setTabText(index, name);
    }
private slots:
    void sendCurrentChanged();
protected:
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);
    
private:
    bool overCloseButton(QPoint mousePos, int left);
    bool overMoreButton(QPoint mousePos, int left);
    int tabWidth(int index,const QFont & font);
    void drawMoreButton(QPainter * painter, int index);

    QStringList _tabsName;
    QList<WidgetFile *> _widgets;
    QList<int> _tabsNameWidth;
    bool _currentChangedSignalWillBeSend;
    int _currentIndex;
    int _widgetOverId;
    int _overCloseId;
    int _overMoreId;
    int _pressCloseId;
    int _pressMoreId;
    int _padding;
    int _margin;
    int _closeLeftMargin ;
    int _moreRightMargin;
    int _closeWidth;
    int _moreWidth;
};

#endif // WIDGETTAB_H
