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
        QStandardItem * i = new QStandardItem(macroName);
        i->setData(macroName);
        it << i << new QStandardItem("-");
        parentItem->appendRow(it);
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
            QStandardItem * i = new QStandardItem(macroName);
            i->setData(dirName+"/"+macroName);
            it << i << new QStandardItem("-");
            upItem->appendRow(it);
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
    connect(ui->lineEditDescription, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->lineEditKeys, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->lineEditLeftWord, SIGNAL(textChanged(QString)), this, SLOT(setModified(QString)));
    connect(ui->checkBoxReadOnly, SIGNAL(toggled(bool)), this, SLOT(setMacroReadOnly(bool)));

    connect(_model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));


}

DialogMacros::~DialogMacros()
{
    delete ui;
}

void DialogMacros::closeEvent(QCloseEvent *)
{
    saveLastClickedItem();
}

void DialogMacros::onItemChanged(QStandardItem* item)
{
    qDebug()<<"rename "<<item->data().toString()<<" -> "<<item->text()<<" "<<MacroEngine::Instance.rename(item->data().toString(), item->text());
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
    /*ui->lineEditKeys->setReadOnly(macro.readOnly);
    ui->lineEditLeftWord->setReadOnly(macro.readOnly);*/

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
