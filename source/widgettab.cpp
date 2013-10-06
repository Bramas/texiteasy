#include "widgettab.h"
#include <QPainter>
#include <QMouseEvent>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QDebug>
#include <QFontMetrics>
#include "configmanager.h"
#include "widgetfile.h"

WidgetTab::WidgetTab(QWidget *parent) :
    QWidget(parent)
{
    _currentIndex = -1;
    this->setMinimumHeight(30);
    this->setMaximumHeight(30);
    this->setMouseTracking(true);

    _overCloseId = -1;

    _padding = 10;
    _margin = 5;
    _closeLeftMargin = 7;
    _closeWidth = 5;
}

void WidgetTab::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QFontMetrics fm(painter.font());

    QPen defaultPen(QColor(180,180,180));
    QPen hoverPen(QColor(220,220,220));
    QPen defaultClosePen(QColor(100,100,100));
    defaultClosePen.setWidth(2);
    QPen hoverClosePen(QColor(130,130,130));
    hoverClosePen.setWidth(2);
    QPen overClosePen(QColor(160,160,160));
    overClosePen.setWidth(2);


    QPen rectPen(QColor(0,0,0));
    QBrush backgroundBrush(QColor(10,10,10));
    QBrush defaultRectBrush(QColor(40,40,40));
    QBrush hoverRectBrush(ConfigManager::Instance.getTextCharFormats("linenumber").background().color());


    painter.setBrush(backgroundBrush);
    painter.drawRect(-1, -1, width() + 2, height() + 2);

    _tabsNameWidth.clear();
    _tabsNameWidth.append(0);
    int index = 0;
    int cummulatedWidth = 10;

    foreach(QString tabName, _tabsName)
    {

        if(index == this->currentIndex())
        {
            painter.setBrush(hoverRectBrush);
        }
        else
        {
            painter.setBrush(defaultRectBrush);
        }
        painter.setPen(rectPen);
        painter.drawRoundedRect(cummulatedWidth, 5, fm.width(tabName) + _padding * 2 + _closeLeftMargin + _closeWidth, 30, 5, 5);

        if(index == this->currentIndex())
        {
            painter.setPen(hoverPen);
        }
        else
        {
            painter.setPen(defaultPen);
        }
        painter.drawText(cummulatedWidth + _padding, 22, tabName);

        if(index == _overCloseId)
        {
            painter.setPen(overClosePen);
        }
        else
        if(index == this->currentIndex())
        {
            painter.setPen(hoverClosePen);
        }
        else
        {
            painter.setPen(defaultClosePen);
        }
        painter.drawLine(cummulatedWidth + fm.width(tabName) + _padding + _closeLeftMargin, 15, cummulatedWidth + fm.width(tabName) + _padding + _closeLeftMargin + _closeWidth, 15 + _closeWidth);
        painter.drawLine(cummulatedWidth + fm.width(tabName) + _padding + _closeLeftMargin, 15 + _closeWidth, cummulatedWidth + fm.width(tabName) + _padding + _closeLeftMargin + _closeWidth, 15);


        cummulatedWidth += fm.width(tabName) + _padding * 2 + _closeLeftMargin + _closeWidth + _margin;
        _tabsNameWidth.append(cummulatedWidth);
        ++index;
    }


}

void WidgetTab::mousePressEvent(QMouseEvent * event)
{
    int idx = -1;
    foreach(int w, _tabsNameWidth)
    {
        if(idx == -1 && event->pos().x() < w)
        {
            return;
        }
        if(event->pos().x() < w)
        {
            if(this->overCloseButton(event->pos(), w))
            {
                emit tabCloseRequested(idx);
                return;
            }
            setCurrentIndex(idx);
            return;
        }
        ++idx;
    }
    return;
}

bool WidgetTab::overCloseButton(QPoint mousePos, int left)
{
    return mousePos.x() <= left - _padding - _margin + 2 &&
           mousePos.x() >= left - _padding - _margin - _closeWidth - 2 &&
           mousePos.y() >= 15 - 2 &&
           mousePos.y() <= 15 + _closeWidth + 2;
}

void WidgetTab::mouseMoveEvent(QMouseEvent * event)
{
    _overCloseId = -1;
    this->setCursor(Qt::ArrowCursor);
    int idx = -1;
    foreach(int w, _tabsNameWidth)
    {
        if(idx == -1 && event->pos().x() < w)
        {
            break;
        }
        if(event->pos().x() < w)
        {
            if(this->overCloseButton(event->pos(), w))
            {
                this->setCursor(Qt::PointingHandCursor);
                _overCloseId = idx;
                break;
            }
            break;
        }
        ++idx;
    }

    update();
}

void WidgetTab::removeTab(WidgetFile *widget)
{
    int index = _widgets.indexOf(widget);
    this->removeTab(index);
}
void WidgetTab::removeTab(int index)
{
    _widgets.removeAt(index);
    _tabsName.removeAt(index);

    if(this->currentIndex() != index)
    {
        return;
    }
    if(!this->count())
    {
        this->setCurrentIndex(-1);
        return;
    }
    if(index == 0)
    {
        this->setCurrentIndex(0);
        return;
    }
    this->setCurrentIndex(index - 1);
}
