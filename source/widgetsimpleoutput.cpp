#include "widgetsimpleoutput.h"
#include "builder.h"
#include "widgettextedit.h"
#include "latexoutputfilter.h"
#include "widgetfile.h"
#include "mainwindow.h"
#include "filemanager.h"
#include "taskpane/taskwindow.h"

#include <QHeaderView>
#include <QDebug>
#include <QStyle>

WidgetSimpleOutput::WidgetSimpleOutput(QWidget *parent) :
    QTableWidget(0,4,parent)
{
    QStringList headers;
    headers << tr("") << tr("Fichier") << tr("Ligne") << tr("Message");
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
    connect(_builder, SIGNAL(success()),this, SLOT(onError()));
    //connect(_builder, SIGNAL(success()),this, SLOT(onSuccess()));
    connect(_builder, SIGNAL(started()), this, SLOT(clear()));
}


void WidgetSimpleOutput::onError()
{
    LatexOutputFilter f;
    f.run(_builder->output());
    //foreach(const LatexLogEntry &l, f.m_infoList)
    //    qDebug()<<l.type<<" "<<l.file<<" "<<l.logline<<" "<<l.message;

    this->setRowCount(f.m_infoList.count());

    QStringList headers;
    headers << tr("") << tr("Fichier") << tr("Ligne") << tr("Message");
    this->setHorizontalHeaderLabels(headers);
    this->setColumnWidth(0, 30);
    this->setColumnWidth(2, 65);

    int row = 0;
    foreach(const LatexLogEntry &logEntry, f.m_infoList)
    {
        QTableWidgetItem * item = new QTableWidgetItem();
        switch(logEntry.type)
        {
        case LT_ERROR: item->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxCritical)); break;
        case LT_WARNING: item->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxWarning)); break;
        case LT_INFO: item->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxInformation)); break;
        case LT_BADBOX: item->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxInformation)); break;
        }
        item->setTextAlignment(Qt::AlignCenter);
        this->setItem(row,0, item);

        item = new QTableWidgetItem(QFileInfo(logEntry.file).fileName());
        item->setTextAlignment(Qt::AlignCenter);
        item->setData(Qt::StatusTipRole, logEntry.file);
        this->setItem(row,1,item);
        item = new QTableWidgetItem(QString::number(logEntry.oldline));
        item->setTextAlignment(Qt::AlignCenter);
        this->setItem(row,2,item);
        this->setItem(row,3,new QTableWidgetItem(logEntry.message));
        this->item(row,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        this->item(row,1)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        this->item(row,2)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        this->item(row,3)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ++row;
    }
}
void WidgetSimpleOutput::onSuccess()
{

}
void WidgetSimpleOutput::onCellSelected(int row, int)
{
    QString err = this->item(row,3)->text();
    int line = this->item(row,2)->text().toInt();
    QRegExp undefinedCommand("Undefined control sequence.* (\\\\[a-zA-Z]+)");
    QString search("");
    if(err.indexOf(undefinedCommand) != -1)
    {
        search = undefinedCommand.capturedTexts().at(1);
    }
    QString filename = this->item(row,1)->data(Qt::StatusTipRole).toString();
    this->_widgetTextEdit->widgetFile()->window()->open(filename);
    WidgetFile * w = FileManager::Instance.widgetFile(filename);
    if(w){
        w->widgetTextEdit()->goToLine(line,search);
    }
    else
    {
        qDebug()<<"widgetFile "<<filename<<" not found";
    }
}
