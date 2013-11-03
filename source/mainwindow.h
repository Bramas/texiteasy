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
class QGridLayout;
class WidgetTab;
class WidgetFile;

namespace Ui {
class MainWindow;
}

class WidgetEmpty : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetEmpty(QWidget *parent = 0) : QWidget(parent) { }
signals:
    void mouseDoubleClick();
protected:
    void mouseDoubleClickEvent(QMouseEvent * event)
    {
        emit mouseDoubleClick();
        QWidget::mouseMoveEvent(event);
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void setTheme(QString theme);
    bool canBeOpened(QString filename);
    bool canBeInserted(QString filename);
    ~MainWindow();

public slots:

    void newFile();
    void open();
    void open(QString filename);
    void openLast(void);
    void clearLastOpened(void);
    void focus(void);
    void changeTheme(void);
    void onCurrentFileChanged(WidgetFile * widget);
    bool closeTab(int);
    void addFilenameToLastOpened(QString filename);
    void initBuildMenu();
private slots:
    void addUpdateMenu();
protected:
    void closeEvent(QCloseEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);
    void changeEvent(QEvent *event);
    
private:
    void closeCurrentWidgetFile();

    Ui::MainWindow *ui;
    void initTheme();

    DialogConfig * dialogConfig;
    DialogWelcome * dialogWelcome;
    WidgetTab * _tabWidget;
    QVBoxLayout * _verticalLayout;
    WidgetStatusBar * _widgetStatusBar;
    WidgetEmpty * _emptyWidget;
};

#endif // MAINWINDOW_H
