/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                          *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.       *                         *
 *                                                                         *
 ***************************************************************************/

#include "widgetinsertcommand.h"
#include "ui_widgetinsertcommand.h"
#include "widgettextedit.h"

WidgetInsertCommand::WidgetInsertCommand(WidgetTextEdit *parent) :
    QWidget(parent),
    ui(new Ui::WidgetInsertCommand)
{
    ui->setupUi(this);
    this->setVisible(false);
    this->ui->tableWidget->item(0,0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

WidgetInsertCommand::~WidgetInsertCommand()
{
    delete ui;
}
