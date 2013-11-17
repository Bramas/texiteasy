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


#include "dialogaddlatexcommand.h"
#include "ui_dialogaddlatexcommand.h"
#include "configmanager.h"

DialogAddLatexCommand::DialogAddLatexCommand(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddLatexCommand)
{
    ui->setupUi(this);
    this->ui->comboBox->addItem(trUtf8("Personalisé"),"");

    int i = 0;
    foreach(const QString& name, ConfigManager::DefaultLatexCommandNames)
    {
        this->ui->comboBox->addItem(name, ConfigManager::DefaultLatexCommands.at(i++));
    }

    connect(this->ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentItemChanged(int)));
}

DialogAddLatexCommand::~DialogAddLatexCommand()
{
    delete ui;
}
void DialogAddLatexCommand::selectCustomItem()
{
    ui->comboBox->setCurrentIndex(0);
}

void DialogAddLatexCommand::onCurrentItemChanged(int index)
{
    if(index == 0)
    {
        disconnect(this->ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
        disconnect(this->ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
        return;
    }
    this->ui->lineEdit_name->setText(this->ui->comboBox->itemText(index));
    this->ui->lineEdit->setText(this->ui->comboBox->itemData(index).toString());

    connect(this->ui->lineEdit, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
    connect(this->ui->lineEdit_name, SIGNAL(textChanged(QString)), this, SLOT(selectCustomItem()));
}

QString DialogAddLatexCommand::command()
{
    return ui->lineEdit->text();
}
QString DialogAddLatexCommand::name()
{
    return ui->lineEdit_name->text();
}
