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

#ifndef WIDGETINSERTCOMMAND_H
#define WIDGETINSERTCOMMAND_H

#include <QWidget>
#include <QStringList>
class QTableWidgetItem;


class WidgetTextEdit;
namespace Ui {
class WidgetInsertCommand;
}

class WidgetInsertCommand : public QWidget
{
    Q_OBJECT
    
public:
    ~WidgetInsertCommand();

    static WidgetInsertCommand * instance() { if(!_instance) _instance = new WidgetInsertCommand(); return _instance; }

    void setParent(WidgetTextEdit* parent);

signals:
    void commandActivated(QString);
    
public slots:
    void onCellActivated(QTableWidgetItem*);

private:
    explicit WidgetInsertCommand();
    Ui::WidgetInsertCommand *ui;
    static WidgetInsertCommand * _instance;

    WidgetTextEdit * _widgetTextEdit;

    QStringList _tabslabel;

    void saveCommandsToPng();
    int mathEnvIndex;
    int commandIndex;
    int groupIndex;
};

#endif // WIDGETINSERTCOMMAND_H
