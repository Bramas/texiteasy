#include "paneautocorrector.h"
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QSizePolicy>
#include <QTextBlock>
#include <QScrollBar>
#include <QAction>
#include "widgetfile.h"
#include "builder.h"
#include "filemanager.h"

PaneAutoCorrector::PaneAutoCorrector(WidgetFile * widgetFile) :
    _builder(0),
    _widgetFile(widgetFile)
{
    _mainWidget = new QPlainTextEdit();
    _mainWidget->setReadOnly(true);

    _action = new QAction(statusbarText(), 0);
    _action->setCheckable(true);
    _height = 100;
    _collapsed = true;
    this->updateBuilder();
    connect(_widgetFile, SIGNAL(opened()), this, SLOT(updateBuilder()));
}

PaneAutoCorrector::~PaneAutoCorrector()
{

}


QWidget * PaneAutoCorrector::paneWidget()
{
    return _mainWidget;
}

QObject * PaneAutoCorrector::getQObject()
{
    return this;
}

void PaneAutoCorrector::updateBuilder()
{
    this->_builder = _widgetFile->file()->builder();
    if(!this->_builder || !FileManager::Instance.currentWidgetFile())
    {
        return;
    }

    connect(_builder, SIGNAL(error()),this, SLOT(onError()));
    connect(_builder, SIGNAL(started()), _mainWidget, SLOT(clear()));
    connect(_builder, SIGNAL(outputUpdated(QString)), this, SLOT(setOutput(QString)));
}

void PaneAutoCorrector::openMyPane()
{
    if(FileManager::Instance.currentWidgetFile())
    {
        FileManager::Instance.currentWidgetFile()->openPane(this);
    }
}
void PaneAutoCorrector::closeMyPane()
{
    if(FileManager::Instance.currentWidgetFile())
    {
        FileManager::Instance.currentWidgetFile()->closePane(this);
    }
}

void PaneAutoCorrector::mousePressEvent(QMouseEvent *event)
{

}

void PaneAutoCorrector::expand()
{
    if(_collapsed)
    {
        _collapsed = false;
    }
}

void PaneAutoCorrector::collapsed()
{
    if(!_collapsed)
    {
        _collapsed = true;
    }
}
void PaneAutoCorrector::onError()
{
    this->expand();
}
void PaneAutoCorrector::onSuccess()
{
    this->collapsed();
}

void PaneAutoCorrector::setOutput(QString newText)
{
    bool atEnd = false;
    if(_mainWidget->verticalScrollBar()->value() == _mainWidget->verticalScrollBar()->maximum())
    {
        atEnd = true;
    }
    _mainWidget->setPlainText(newText);
    if(atEnd)
    {
        _mainWidget->verticalScrollBar()->setValue(_mainWidget->verticalScrollBar()->maximum());
    }
}
