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

#ifndef DIALOGMACRO_H
#define DIALOGMACRO_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class DialogMacros;
}
class QStandardItemModel;
class DialogMacros : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMacros(QWidget *parent = 0);
    ~DialogMacros();

public slots:
    void onClicked(QModelIndex index);
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void setModified(bool b = true) { _modified = b; }
    void setModified(QString) { _modified = true; }
private:
    void saveLastClickedItem();
    void loadMacro(QString name);
private:
    Ui::DialogMacros *ui;
    QString _macrosPath;
    QStandardItemModel * _model;
    QModelIndex _lastClickedItem;
    bool _modified;
};

#endif // DIALOGMACRO_H
