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

#include "dialogwelcome.h"
#include "ui_dialogwelcome.h"

DialogWelcome::DialogWelcome(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogWelcome)
{
    ui->setupUi(this);
}

DialogWelcome::~DialogWelcome()
{
    delete ui;
}
QPushButton * DialogWelcome::getActionNew()
{
    return this->ui->pushButtonNew;
}
QPushButton * DialogWelcome::getActionOpen()
{
    return this->ui->pushButtonOpen;
}
QPushButton * DialogWelcome::getActionOpenLast()
{
    return this->ui->pushButtonOpenLast;
}
