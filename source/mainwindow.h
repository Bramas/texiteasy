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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QMimeData;
class WidgetTextEdit;
class WidgetLineNumber;
class WidgetScroller;
class WidgetPdfViewer;
class WidgetConsole;
class WidgetSimpleOutput;
class MiniSplitter;
class DialogWelcome;
class DialogConfig;
class QVBoxLayout;
class SyntaxHighlighter;
class WidgetFindReplace;
class WidgetStatusBar;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    bool closeCurrentFile();
    void setTheme(QString theme);
    ~MainWindow();

public slots:

    void newFile(void);
    void open(QString filename = "");
    void openLast(void);
    void clearLastOpened(void);
    void focus(void);
    void changeTheme(void);
protected:
    void closeEvent(QCloseEvent *);
    
private:
    Ui::MainWindow *ui;
    void initTheme();

    DialogConfig * dialogConfig;
    DialogWelcome * dialogWelcome;
    WidgetStatusBar * _widgetStatusBar;
};

#endif // MAINWINDOW_H
