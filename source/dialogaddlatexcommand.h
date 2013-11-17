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



#ifndef DIALOGADDLATEXCOMMAND_H
#define DIALOGADDLATEXCOMMAND_H

#include <QDialog>

namespace Ui {
class DialogAddLatexCommand;
}

class DialogAddLatexCommand : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddLatexCommand(QWidget *parent = 0);
    ~DialogAddLatexCommand();
    QString command();
    QString name();

private slots:
    void onCurrentItemChanged(int index);
    void selectCustomItem();
private:
    Ui::DialogAddLatexCommand *ui;
};

#endif // DIALOGADDLATEXCOMMAND_H
