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

/**
 * @file mainwindow.h
 * @brief The main window
 * @author Quentin BRAMAS
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIcon>

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
class QMessageBox;

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

/**
 * @class MainWindow
 * @brief The MainWindow class is the main class that call all other widgets. it contains all the menus.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    void setTheme(QString theme);
    bool canBeOpened(QString filename);
    bool canBeInserted(QString filename);
    bool handleMimeData(const QMimeData* mimeData);
    QAction * actionByRole(QString actionRole);
    ~MainWindow();

public slots:
    void proposeUpdateDialog();
    void setWindowModified(bool b);
    WidgetFile *newFile();
    void open();
    void open(QString filename, int cursorPosition = 0, QPoint pdfPosition = QPoint(0,0), qreal pdfZoom = 1);
    void openLast(void);
    void openLastSession(void);
    void clearLastOpened(void);
    void focus(void);
    void changeTheme(void);
    void onCurrentFileChanged(WidgetFile * widget);
    /**
     * @brief   The tab may not be close because the user does not want to
     *          close a modified file. If the user want to save a new file, filename will contains the filename of the new created file (there is no other way to found out where the user saved the file)
     * @param filename
     * @return true if the tab is closed, false otherwise.
     */
    bool closeTab(int, QString **filename = 0);
    /**
     * @brief if the file is modified, ask the user what he want to do
     * @param widget
     * @return true if the widget can be closed safely
     */
    bool widgetFileCanBeClosed(WidgetFile * widget);
    void addFilenameToLastOpened(QString filename);
    void onFilenameChanged(QString filename);
    void initBuildMenu();
    void initMacrosMenu();
    void setUtf8();
    void setOtherEncoding();
    void reopenWithEncoding();
    void insertTexDirEncoding();
    void insertTexDirProgram();
    void insertTexDirSpellCheck();
    void insertTexDirRoot();
    /**
     * @brief onOtherInstanceMessage
     */
    void onOtherInstanceMessage(const QString &);
    void openSendFeedbackDialog();

private slots:
    void addUpdateMenu();
protected:
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *);
    void changeEvent(QEvent *event);
    void createLastOpenedFilesMenu();
    
private:
    void closeCurrentWidgetFile();

    Ui::MainWindow *ui;
    void initTheme();

    QMessageBox * _confirmCloseMessageBox;
    DialogConfig * dialogConfig;
    DialogWelcome * dialogWelcome;
    WidgetTab * _tabWidget;
    QVBoxLayout * _verticalLayout;
    WidgetStatusBar * _widgetStatusBar;
    WidgetEmpty * _emptyWidget;
    QAction * _menuMacrosAction;
};

#endif // MAINWINDOW_H
