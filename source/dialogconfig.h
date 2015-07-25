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


#ifndef DIALOGCONFIG_H
#define DIALOGCONFIG_H

#include <QDialog>
#include <QSettings>
#include <QList>

namespace Ui {
    class DialogConfig;
}
class QListWidgetItem;
class MainWindow;
class QTableWidgetItem;

class DialogConfig : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogConfig(MainWindow * parent = 0);
    void addEditableActions(const QList<QAction *> & actions);
    ~DialogConfig();
public slots:
    void selectBinDirectory(void);
    void selectSvnDirectory(void);
    void save(void);
    void saveAndClose(void);
    void show();
    void changePage(int currentRow);
    void configureShortCut(QTableWidgetItem *item);
    void addNewCommand();
    void onCurrentLatexCommandChanged();
    void deleteSelectedLatex();
    void updateComboboxDefaultLatex();
    
protected:
    void changeEvent(QEvent *event);
private:
    void initShortcutsTable();
    QList<QAction*> _actionsList;
    Ui::DialogConfig *ui;
    QSettings settings;
    MainWindow * _mainWindows;
    QString currentLatexName;
};

#endif // DIALOGCONFIG_H
