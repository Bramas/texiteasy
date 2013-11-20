/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/


#include "dialogmacros.h"
#include "ui_dialogmacros.h"
#include "configmanager.h"
#include "macroengine.h"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QDebug>

DialogMacros::DialogMacros(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMacros),
    _modified(false)
{
    ui->setupUi(this);



    _model = new QStandardItemModel;
    _model->setColumnCount(1);
    QStandardItem *parentItem = _model->invisibleRootItem();
    _macrosPath = ConfigManager::Instance.macrosPath();
    QDir dir(_macrosPath);
    QStringList list = dir.entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\"+ConfigManager::MacroSuffix+"$"));
    list.replaceInStrings(QRegExp("\\"+ConfigManager::MacroSuffix+"$"), "");

    QList<QStandardItem*> it;

    foreach(const QString & macroName, list)
    {
        it.clear();
        QStandardItem * i = new QStandardItem(macroName);
        i->setDropEnabled(false);
        i->setDragEnabled(true);
        i->setData(macroName);
        it << i;
        i = new QStandardItem(MacroEngine::Instance.macros().value(macroName).keys);
        i->setDropEnabled(false);
        i->setDragEnabled(false);
        i->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        //it << i;
        parentItem->appendRow(it);
    }

    QMap<QString, QStandardItem> parents;

    QStandardItem * upItem;
    foreach(const Macro & macro, MacroEngine::orderedMacros())
    {
        it.clear();
        if(macro.name.contains('/'))
        {

        }
        upItem = new QStandardItem(trUtf8(dirName.toUtf8().data()));
        upItem->setData(dirName);
        upItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        upItem->setDropEnabled(true);
        upItem->setDragEnabled(false);
        QStandardItem * i = new QStandardItem("");
        i->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        i->setDropEnabled(false);
        i->setDragEnabled(false);
        it << upItem;
        //it << i;
        parentItem->appendRow(it);

        QStringList subList = QDir(_macrosPath+dirName).entryList(QDir::Files | QDir::Readable, QDir::Name).filter(QRegExp("\\"+ConfigManager::MacroSuffix+"$"));
        subList.replaceInStrings(QRegExp("\\"+ConfigManager::MacroSuffix+"$"), "");
        foreach(const QString & macroName, subList)
        {
            it.clear();
            i = new QStandardItem(macroName);
            i->setDropEnabled(false);
            i->setDragEnabled(true);
            i->setData(dirName+"/"+macroName);
            it << i;
            i = new QStandardItem(MacroEngine::Instance.macros().value(dirName+"/"+macroName).keys);
            i->setDropEnabled(false);
            i->setDragEnabled(false);
            i->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            //it << i;
            upItem->appendRow(it);
        }
    }
    //ui->tree->header()->setStretchLastSection(true);



    // Provide translation for default folder
    trUtf8("Document");
    trUtf8("Structure");
    trUtf8("Mathematics");
    trUtf8("List");
    trUtf8("Table");
    trUtf8("Figure");
    ui->tree->setModel(_model);

    QHeaderView * h = ui->tree->header();
    /*h->resizeSection(0,ui->tree->width()-75);
    h->resizeSection(1, 74);
    h->setStretchLastSection(true);
    ui->tree->setHeader(h);*/

    ui->tree->setDragEnabled(true);
    ui->tree->setDropIndicatorShown(true);
    ui->tree->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tree->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(this->ui->tree, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), this, SLOT(setModified()));
    connect(ui->lineEditDescription, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->lineEditKeys, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->lineEditLeftWord, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->checkBoxReadOnly, SIGNAL(toggled(bool)), this, SLOT(setMacroReadOnly(bool)));

    connect(_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
    connect(ui->pushButtonNewMacro, SIGNAL(clicked()), this, SLOT(onNewItemRequested()));


    //connect(_model, SIGNAL())


}

DialogMacros::~DialogMacros()
{
    delete ui;
}

void DialogMacros::closeEvent(QCloseEvent *)
{
    saveLastClickedItem();
    saveOrder();
}

void DialogMacros::onItemChanged(QStandardItem* item)
{
    if(item->text().contains(QRegExp("[\"\\*:<>?\\\\\\/\\|]")))
    {
        QMessageBox::warning(this, trUtf8("Wrong name."), trUtf8("Macro name cannot contains the following characters : \"*:<>?\\/|"));
        QString name = item->data().toString();
        if(name.contains('/'))
        {
            name = name.split('/').at(1);
        }
        item->setText(name);
        return;
    }
    QStandardItem * parent = item->parent();
    QString newName = item->text();
    if(parent)
    {
        newName = parent->data().toString()+"/"+newName;
    }
    if(MacroEngine::Instance.rename(item->data().toString(), newName))
    {
        item->setData(newName);
    }
}
void DialogMacros::onNewItemRequested()
{
    qDebug()<<"new Item";
    QStandardItem * item = new QStandardItem();
    _model->appendRow(item);
    ui->tree->setCurrentIndex(_model->indexFromItem(item));
    ui->tree->edit(_model->indexFromItem(item));
    return;
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
    {
        saveLastClickedItem();
    }


    // load the new macro
    QString name;
    if(item->parent() && item->parent() != _model->invisibleRootItem())
    {
        name += item->parent()->data().toString()+"/";
    }
    name += item->text();
    loadMacro(name);
    _lastClickedItem = _model->indexFromItem(item);
}

void DialogMacros::setMacroReadOnly(bool readOnly)
{
    ui->plainTextEdit->setReadOnly(readOnly);
    ui->lineEditDescription->setReadOnly(readOnly);
}

void DialogMacros::loadMacro(QString name)
{
    Macro macro = MacroEngine::Instance.macros().value(name);
    ui->plainTextEdit->setPlainText(macro.content);
    ui->lineEditDescription->setText(macro.description);
    ui->lineEditKeys->setText(macro.keys);
    ui->lineEditLeftWord->setText(macro.leftWord);
    _modified = false;

    ui->checkBoxReadOnly->setChecked(macro.readOnly);
    ui->plainTextEdit->setReadOnly(macro.readOnly);
    ui->lineEditDescription->setReadOnly(macro.readOnly);

}
void DialogMacros::saveOrder()
{
    QStringList macrosOrder;
    for(int idx = 0; idx < _model->rowCount(); ++idx)
    {
        QStandardItem * item = _model->item(idx);
        if(item->hasChildren())
        {
            for(int sub_idx = 0; sub_idx < item->rowCount(); ++sub_idx)
            {
                QStandardItem * subitem = item->child(sub_idx);
                onItemChanged(subitem);
                macrosOrder << subitem->data().toString();
            }
        }
        else
        {
            onItemChanged(item);
            macrosOrder << item->data().toString();
        }
    }
    QSettings settings;
    settings.beginGroup("Macros");
    settings.setValue("macrosOrder", macrosOrder);
}

void DialogMacros::saveLastClickedItem()
{
    if(!_lastClickedItem.isValid() || !_modified)
    {
        return;
    }

    QStandardItem * lastItem = _model->itemFromIndex(_lastClickedItem);
    QString name;
    if(lastItem->parent() && lastItem->parent() != _model->invisibleRootItem())
    {
        name += lastItem->parent()->data().toString()+"/";
    }
    name += lastItem->text();
    MacroEngine::Instance.saveMacro(name,
                                    ui->lineEditDescription->text(),
                                    ui->lineEditKeys->text(),
                                    ui->lineEditLeftWord->text(),
                                    ui->plainTextEdit->toPlainText().toUtf8());
}
