#include "dialogmacros.h"
#include "ui_dialogmacros.h"
#include "configmanager.h"

#include <QStandardItemModel>
#include <QDebug>

DialogMacros::DialogMacros(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMacros),
    _modified(false)
{
    ui->setupUi(this);



    _model = new QStandardItemModel;
    _model->setColumnCount(2);
    QStandardItem *parentItem = _model->invisibleRootItem();
    _macrosPath = ConfigManager::Instance.macrosPath();
    QDir dir(_macrosPath);
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\"+ConfigManager::MacroSuffix+"$"));
    list.replaceInStrings(QRegExp("\\"+ConfigManager::MacroSuffix+"$"), "");

    QList<QStandardItem*> it;

    foreach(const QString & macroName, list)
    {
        it.clear();
        it << new QStandardItem(macroName) << new QStandardItem("-");
        parentItem->appendRow(it);
        qDebug()<<macroName<<it.first()->row();
    }

    list = dir.entryList(QDir::Dirs | QDir::Readable | QDir::NoDotAndDotDot, QDir::Name);
    QStandardItem * upItem;
    foreach(const QString & dirName, list)
    {
        upItem = new QStandardItem(trUtf8(dirName.toUtf8().data()));
        upItem->setData(dirName);
        upItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        parentItem->appendRow(upItem);

        QStringList subList = QDir(_macrosPath+dirName).entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\"+ConfigManager::MacroSuffix+"$"));
        subList.replaceInStrings(QRegExp("\\"+ConfigManager::MacroSuffix+"$"), "");
        foreach(const QString & macroName, subList)
        {
            it.clear();
            it << new QStandardItem(macroName) << new QStandardItem("-");
            upItem->appendRow(it);
            qDebug()<<macroName<<it.first()->row();
        }
    }



    // Provide translation for default folder
    trUtf8("Document");
    trUtf8("Structure");
    trUtf8("Mathématiques");
    trUtf8("Listes");
    trUtf8("Tableau");
    trUtf8("Figure");
    ui->tree->setModel(_model);


    connect(this->ui->tree, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), this, SLOT(setModified()));
}

DialogMacros::~DialogMacros()
{
    delete ui;
}

void DialogMacros::closeEvent(QCloseEvent *)
{
    saveLastClickedItem();
}


void DialogMacros::onClicked(QModelIndex index)
{
    QStandardItem * itemParent;
    if(index.parent().isValid())
    {
        itemParent = _model->itemFromIndex(index.parent());
    }
    else
    {
        itemParent = _model->invisibleRootItem();
    }
    QStandardItem * item = itemParent->child(index.row());
    if(!(item->flags() & Qt::ItemIsEditable))
    {
        return;
    }

    // save the previous macro
    saveLastClickedItem();


    // load the new macro
    {
        QString filename = _macrosPath;
        if(item->parent() && item->parent() != _model->invisibleRootItem())
        {
            filename += item->parent()->data().toString()+"/";
        }
        filename += item->text()+ConfigManager::MacroSuffix;
        QFile file(filename);
        qDebug()<<"load : "<<filename;
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            ui->plainTextEdit->setPlainText(file.readAll());
            _modified = false;
        }
    }
    _lastClickedItem = _model->indexFromItem(item);
}

void DialogMacros::saveLastClickedItem()
{
    if(_lastClickedItem.isValid() && _modified)
    {
        QStandardItem * lastItem = _model->itemFromIndex(_lastClickedItem);
        QString filename = _macrosPath;
        if(lastItem->parent() && lastItem->parent() != _model->invisibleRootItem())
        {
            filename += lastItem->parent()->data().toString()+"/";
        }
        filename += lastItem->text()+ConfigManager::MacroSuffix;
        QFile file(filename);
        qDebug()<<"save : "<<filename;
        if(file.open(QFile::WriteOnly | QFile::Text))
        {
            qDebug()<<"write : "<<ui->plainTextEdit->toPlainText().toUtf8();
            file.write(ui->plainTextEdit->toPlainText().toUtf8());
        }
    }
}
