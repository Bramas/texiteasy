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
#include <QMenu>
#include <QTreeView>


class TreeView : public QTreeView
{
public:
    TreeView(QWidget * parent) : QTreeView(parent) { }
protected:
    void keyPressEvent(QKeyEvent *event)
    {
        QTreeView::keyPressEvent(event);
        if(currentIndex().isValid())
        {
            emit pressed(currentIndex());
        }
    }
};



DialogMacros::DialogMacros(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMacros),
    _modified(false)
{
    ui->setupUi(this);
    QRect geo = ui->tree->geometry();
    ui->tree->deleteLater();
    ui->tree = new TreeView(this);
    ui->tree->setGeometry(geo);

    _model = new QStandardItemModel;
    _model->setColumnCount(1);
    _macrosPath = ConfigManager::Instance.macrosPath();


    QList<QStandardItem*> it;
    QMap<QString, QStandardItem *> parents;
    QString macroText;
    QStandardItem * upItem;
    QStandardItem * item;
    foreach(const Macro & macro, MacroEngine::Instance.orderedMacros())
    {
        upItem = _model->invisibleRootItem();
        it.clear();
        macroText = macro.name;
        if(macro.name.contains('/'))
        {
            QStringList l = macro.name.split('/');
            QString folder = l.at(0);
            macroText = l.at(1);
            if(!parents.contains(folder))
            {
                upItem = new QStandardItem(trUtf8(folder.toUtf8().data()));
                upItem->setData(folder);
                upItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                upItem->setDropEnabled(true);
                upItem->setDragEnabled(false);
                parents.insert(folder, upItem);
                _model->appendRow(upItem);
            }
            else
            {
                upItem = parents.value(folder);
            }
        }

        if(macro.name.contains('\\'))
        {
            //empty macro
            continue;
        }
        it.clear();
        item = new QStandardItem(macroText);
        item->setDropEnabled(false);
        item->setDragEnabled(true);
        item->setData(macro.name);
        it << item;
        upItem->appendRow(it);

    }

    // Provide translation for default folder
    trUtf8("Document");
    trUtf8("Structure");
    trUtf8("Mathematics");
    trUtf8("List");
    trUtf8("Table");
    trUtf8("Figure");
    ui->tree->setModel(_model);


    ui->tree->setDragEnabled(true);
    ui->tree->setDropIndicatorShown(true);
    ui->tree->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tree->setSelectionBehavior(QAbstractItemView::SelectRows);

    //connect(this->ui->tree, SIGNAL(clicked(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    connect(this->ui->tree, SIGNAL(pressed(QModelIndex)), this, SLOT(onClicked(QModelIndex)));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), this, SLOT(setModified()));
    connect(ui->lineEditDescription, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->lineEditKeys, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->lineEditLeftWord, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->checkBoxReadOnly, SIGNAL(toggled(bool)), this, SLOT(setMacroReadOnly(bool)));

    connect(_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
    connect(ui->pushButtonNewMacro, SIGNAL(clicked()), this, SLOT(onNewItem()));
    connect(ui->pushButtonNewFolder, SIGNAL(clicked()), this, SLOT(onNewFolder()));
    connect(ui->pushButtonDeleteMacro, SIGNAL(clicked()), this, SLOT(deleteCurrent()));

    loadMacro("");

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
    if(!(item->flags() & Qt::ItemIsEditable))
    {
        //it is a folder. Item has been changed internaly.
        return;
    }
    if(item->text().isEmpty() && item->data().toString().isEmpty())
    {
        _model->removeRow(item->row(), _model->indexFromItem(item->parent()));
        return;
    }
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
        loadMacro(newName);
    }

}
void DialogMacros::onNewItem()
{
    QStandardItem * item = new QStandardItem();
    item->setDropEnabled(false);
    item->setDragEnabled(true);
    _model->appendRow(item);
    ui->tree->setCurrentIndex(_model->indexFromItem(item));
    ui->tree->edit(_model->indexFromItem(item));
    return;
}
void DialogMacros::onNewFolder()
{
    QStandardItem * upItem = new QStandardItem();
    upItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    upItem->setDropEnabled(true);
    upItem->setDragEnabled(false);

    _model->appendRow(upItem);
    ui->tree->setCurrentIndex(_model->indexFromItem(upItem));
    onClicked(_model->indexFromItem(upItem));
    ui->lineEditDescription->setFocus();
    _modified = true;
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


    // save the previous macro
    {
        saveLastClickedItem();
        _lastClickedItem = _model->indexFromItem(item);
    }

    if(!(item->flags() & Qt::ItemIsEditable))
    {
        //set Empty content
        loadFolder(item->data().toString());
        ui->pushButtonDeleteMacro->setText(trUtf8("Delete folder"));
        ui->pushButtonDeleteMacro->setEnabled(!item->hasChildren());
        return;
    }
    else
    {
        // load the new macro
        QString name;
        if(item->parent() && item->parent() != _model->invisibleRootItem())
        {
            name += item->parent()->data().toString()+"/";
        }
        name += item->text();
        loadMacro(name);

        ui->pushButtonDeleteMacro->setText(trUtf8("Delete macro"));
        ui->pushButtonDeleteMacro->setEnabled(true);
    }

}

void DialogMacros::setMacroReadOnly(bool readOnly)
{
    ui->plainTextEdit->setReadOnly(readOnly);
    ui->lineEditDescription->setReadOnly(readOnly);
}
void DialogMacros::loadFolder(QString name)
{
    ui->plainTextEdit->setPlainText("");
    ui->lineEditDescription->setText(trUtf8(name.toUtf8().data()));
    ui->lineEditKeys->setText("");
    ui->lineEditLeftWord->setText("");

    ui->label->setText(trUtf8("Name:"));
    _modified = false;


    ui->lineEditDescription->setReadOnly(false);
    ui->lineEditDescription->setEnabled(true);
    ui->plainTextEdit->setEnabled(false);
    ui->lineEditKeys->setEnabled(false);
    ui->lineEditLeftWord->setEnabled(false);
    ui->checkBoxReadOnly->setEnabled(false);
}
void DialogMacros::loadMacro(QString name)
{
    ui->label->setText(trUtf8("Description:"));
    Macro macro = MacroEngine::Instance.macros().value(name);
    ui->plainTextEdit->setPlainText(macro.content);
    ui->lineEditDescription->setText(macro.description);
    ui->lineEditKeys->setText(macro.keys);
    ui->lineEditLeftWord->setText(macro.leftWord);
    _modified = false;

    ui->checkBoxReadOnly->setChecked(macro.readOnly);
    ui->plainTextEdit->setReadOnly(macro.readOnly);
    ui->lineEditDescription->setReadOnly(macro.readOnly);

    bool enabled = !macro.name.isEmpty();
    ui->plainTextEdit->setEnabled(enabled);
    ui->lineEditDescription->setEnabled(enabled);
    ui->lineEditKeys->setEnabled(enabled);
    ui->lineEditLeftWord->setEnabled(enabled);
    ui->checkBoxReadOnly->setEnabled(enabled);
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
void DialogMacros::renameAllChildren(QStandardItem * item, QString newFolderName)
{
    if(item->hasChildren())
    {
        for(int sub_idx = 0; sub_idx < item->rowCount(); ++sub_idx)
        {
            QStandardItem * subitem = item->child(sub_idx);
            QString name = subitem->data().toString();
            name.replace(QRegExp("^[^\\/]*\\/"), newFolderName+"/");
            subitem->setData(name);
        }
    }

}

void DialogMacros::saveLastClickedItem()
{
    if(!_lastClickedItem.isValid() || !_modified)
    {
        return;
    }


    QStandardItem * lastItem = _model->itemFromIndex(_lastClickedItem);

    if(!(lastItem->flags() & Qt::ItemIsEditable))
    {
        QString newName = ui->lineEditDescription->text();
        if(MacroEngine::Instance.renameFolder(lastItem->data().toString(),newName
                                        ))
        {
            _modified = false;
            lastItem->setData(newName);
            lastItem->setText(newName);
            renameAllChildren(lastItem, newName);
        }
        else
        {
            if(lastItem->data().toString().isEmpty())
            {
                _model->removeRow(lastItem->row(), _model->indexFromItem(lastItem->parent()));
            }
        }
    }
    else
    {
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
    _modified = false;
}

void DialogMacros::deleteCurrent()
{
    QStandardItem * item = _model->itemFromIndex(ui->tree->currentIndex());
    QString name = item->data().toString();
    if(item->hasChildren())
    {
        QMessageBox::warning(this, trUtf8("Cannot delete folders."), trUtf8("You cannot delete folder %1, because it contains macros.")
                                        .arg(item->text()));
        return;
    }
    if(QMessageBox::information(this, trUtf8("Delete?"), trUtf8("Are you sure you want to delete %1.")
                                .arg(name), trUtf8("Cancel"), trUtf8("Delete")))
    {
        if(MacroEngine::Instance.deleteMacro(name))
        {
            _model->removeRow(item->row(), _model->indexFromItem(item->parent()));
            if(ui->tree->currentIndex().isValid())
            {
                onClicked(ui->tree->currentIndex());
            }
            else
            {
                loadMacro("");
            }
        }
    }
}

