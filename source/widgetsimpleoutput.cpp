#include "widgetsimpleoutput.h"
#include "builder.h"
#include "widgettextedit.h"
#include <QHeaderView>
#include <QDebug>
#include <QStyle>

WidgetSimpleOutput::WidgetSimpleOutput(QWidget *parent) :
    QTableWidget(0,3,parent)
{
    QStringList headers;
    headers << tr("Type") << tr("Ligne") << tr("Message");
    this->setHorizontalHeaderLabels(headers);
    this->horizontalHeader()->setProperty("stretchLastSection", true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(this,SIGNAL(cellClicked(int,int)), this, SLOT(onCellSelected(int,int)));
}


void WidgetSimpleOutput::setBuilder(Builder *builder)
{
    this->_builder = builder;
    if(!this->_builder)
    {
        return;
    }
    connect(_builder, SIGNAL(error()),this, SLOT(onError()));
    connect(_builder, SIGNAL(success()),this, SLOT(onSuccess()));
    connect(_builder, SIGNAL(started()), this, SLOT(clear()));
}


void WidgetSimpleOutput::onError()
{
    this->setRowCount(this->_builder->simpleOutput().count());

    QStringList headers;
    headers << tr("") << tr("Ligne") << tr("Message");
    this->setHorizontalHeaderLabels(headers);
    this->setColumnWidth(0, 30);
    this->setColumnWidth(1, 65);

    int row = 0;
    foreach(const Builder::Output & outputItem, this->_builder->simpleOutput())
    {
        QTableWidgetItem * item = new QTableWidgetItem();
        item->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxCritical));
        item->setTextAlignment(Qt::AlignCenter);
        this->setItem(row,0, item);
        item = new QTableWidgetItem(outputItem.line);
        item->setTextAlignment(Qt::AlignCenter);
        this->setItem(row,1,item);
        this->setItem(row,2,new QTableWidgetItem(outputItem.message));
        this->item(row,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        this->item(row,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        this->item(row,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ++row;
    }
}
void WidgetSimpleOutput::onSuccess()
{

}
void WidgetSimpleOutput::onCellSelected(int row, int)
{
    QString err = this->item(row,2)->text();
    int line = this->item(row,1)->text().toInt();
    QRegExp undefinedCommand("Undefined control sequence.* (\\\\[a-zA-Z]+)");
    QString search("");
    if(err.indexOf(undefinedCommand) != -1)
    {
        search = undefinedCommand.capturedTexts().at(1);
    }
    _widgetTextEdit->goToLine(line,search);
}
