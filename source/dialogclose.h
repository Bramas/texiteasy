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

#ifndef DIALOGCLOSE_H
#define DIALOGCLOSE_H

#include <QDialog>

namespace Ui {
class DialogClose;
}

class DialogClose : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogClose(QWidget *parent = 0);
    ~DialogClose();
    bool confirmed(void) { return _confirmed; }
    bool saved(void) { return _saved; }
    void setMessage(QString message);

public slots:
    void confirmAndClose(void);
    void saveAndClose(void);
    
private:
    Ui::DialogClose *ui;

    bool _confirmed;
    bool _saved;
};

#endif // DIALOGCLOSE_H
